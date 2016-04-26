
/*****************************************************************************
*
* Copyright (c) 2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Apache License, version 2.0
* http://www.apache.org/licenses/LICENSE-2.0
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/

#include "BlockCrsMatrixWrapper.h" 
#include "BelosWrapper.h" 
#include "PreconditionerFactory.h" 
#include "TrilinosAdapterException.h" 

#include <escript/index.h>
#include <escript/SolverOptions.h>

#include <Kokkos_DefaultNode.hpp>
#include <Tpetra_DefaultPlatform.hpp>
#include <Tpetra_Experimental_BlockCrsMatrix_Helpers.hpp> // for writing
#include <Tpetra_Experimental_BlockVector.hpp>
#include <Tpetra_Vector.hpp>

using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcpFromRef;

namespace esys_trilinos {

template<typename ST>
BlockCrsMatrixWrapper<ST>::BlockCrsMatrixWrapper(const_TrilinosGraph_ptr graph,
                                                 int blocksize) :
    blockSize(blocksize),
    mat(*graph, blocksize)
{
    //mat.fillComplete();

    // initialize column point map, needed by nullifyRowsAndCols to communicate
    // remote values
    typedef Tpetra::Experimental::BlockVector<ST,LO,GO,NT> BlockVector;
    colPointMap = BlockVector::makePointMap(*mat.getColMap(), blockSize);
    std::cout << "Block Matrix has " << mat.getGlobalNumEntries()
              << " entries." << std::endl;
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::fillComplete(bool localOnly)
{
    //RCP<Teuchos::ParameterList> params = Teuchos::parameterList();
    //params->set("No Nonlocal Changes", localOnly);
    //mat.fillComplete(params);
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::add(const std::vector<LO>& rowIdx,
                                    const std::vector<ST>& array)
{
    // NOTE: the reason this method takes a reference to the matrix and
    // we do the following with the row map is to avoid messing with shared
    // pointer use counters given that this method may be called from
    // parallel sections!
    const MapType& rowMap = *mat.getRowMap();
    const size_t emSize = rowIdx.size();
    const LO myLast = rowMap.getMaxLocalIndex();
    std::vector<LO> cols(emSize);
    std::vector<ST> vals(emSize*blockSize*blockSize);
    for (size_t i = 0; i < emSize; i++) {
        const LO row = rowIdx[i];
        if (row <= myLast) {
            for (int j = 0; j < emSize; j++) {
                cols[j] = rowIdx[j];
                for (int k = 0; k < blockSize; k++) {
                    for (int m = 0; m < blockSize; m++) {
                        const size_t srcIdx =
                            INDEX4(k, m, i, j, blockSize, blockSize, emSize);
                        const size_t destIdx =
                            INDEX3(k, m, j, blockSize, blockSize);
                        vals[destIdx] = array[srcIdx];
                    }
                }
            }
            mat.sumIntoLocalValues(row, &cols[0], &vals[0], emSize);
        }
    }
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::ypAx(const Teuchos::ArrayView<ST>& y,
                                   const Teuchos::ArrayView<const ST>& x) const
{
    typedef VectorType<ST> Vector;
    RCP<Vector> X = rcp(new Vector(mat.getDomainMap(), x, x.size(), 1));
    RCP<Vector> Y = rcp(new Vector(mat.getDomainMap(), y, y.size(), 1));

    const ST alpha = Teuchos::ScalarTraits<ST>::one();
    const ST beta = Teuchos::ScalarTraits<ST>::one();

    // Y = beta*Y + alpha*A*X
    mat.apply(*X, *Y, Teuchos::NO_TRANS, alpha, beta);
    Y->get1dCopy(y, y.size());
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::solve(const Teuchos::ArrayView<ST>& x,
                                      const Teuchos::ArrayView<const ST>& b,
                                      escript::SolverBuddy& sb) const
{
    typedef VectorType<ST> Vector;

    RCP<Vector> X = rcp(new Vector(mat.getDomainMap(), 1));
    RCP<Vector> B = rcp(new Vector(mat.getRangeMap(), b, b.size(), 1));
    RCP<const Matrix> A = rcpFromRef(mat);

    if (sb.getSolverMethod() == escript::SO_METHOD_DIRECT) {
        throw TrilinosAdapterException("Amesos2 does not currently support "
                                       "block matrices!");
        /*
        RCP<DirectSolverType<Matrix,Vector> > solver =
                                createDirectSolver<Matrix,Vector>(sb, A, X, B);
        if (sb.isVerbose()) {
            std::cout << solver->description() << std::endl;
            std::cout << "Performing symbolic factorization..." << std::flush;
        }
        solver->symbolicFactorization();
        if (sb.isVerbose()) {
            std::cout << "done\nPerforming numeric factorization..." << std::flush;
        }
        solver->numericFactorization();
        if (sb.isVerbose()) {
            std::cout << "done\nSolving system..." << std::flush;
        }
        solver->solve();
        if (sb.isVerbose()) {
            std::cout << "done" << std::endl;
            RCP<Teuchos::FancyOStream> fos(Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout)));Op
            solver->printTiming(*fos, Teuchos::VERB_HIGH);
        }
*/
    } else { // iterative solver
        RCP<SolverType<ST> > solver = createSolver<ST>(sb);
        RCP<OpType<ST> > prec = createPreconditioner<ST>(A, sb);
        RCP<ProblemType<ST> > problem = rcp(new ProblemType<ST>(A, X, B));

        if (!prec.is_null()) {
            // Trilinos BiCGStab does not currently support left preconditioners
            if (sb.getSolverMethod() == escript::SO_METHOD_BICGSTAB)
                problem->setRightPrec(prec);
            else
                problem->setLeftPrec(prec);
        }
        problem->setProblem();
        solver->setProblem(problem);
        Belos::ReturnType result = solver->solve();
        if (sb.isVerbose()) {
            const int numIters = solver->getNumIters();
            if (result == Belos::Converged) {
                std::cout << "The solver took " << numIters
                   << " iteration(s) to reach a relative residual tolerance of "
                   << sb.getTolerance() << "." << std::endl;
            } else {
                std::cout << "The solver took " << numIters
                   << " iteration(s), but did not reach a relative residual "
                   "tolerance of " << sb.getTolerance() << "." << std::endl;
            }
        }
    }
    X->get1dCopy(x, x.size());
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::nullifyRowsAndCols(
                                  const Teuchos::ArrayView<const ST>& rowMask,
                                  const Teuchos::ArrayView<const ST>& colView,
                                  ST mdv)
{
    RCP<VectorType<ST> > lclCol = rcp(new VectorType<ST>(mat.getRangeMap(),
                                                 colView, colView.size(), 1));
    RCP<MapType> cpm = rcpFromRef(colPointMap);
    RCP<VectorType<ST> > gblCol = rcp(new VectorType<ST>(cpm, 1));

    const ImportType importer(mat.getRangeMap(), cpm);
    gblCol->doImport(*lclCol, importer, Tpetra::INSERT);
    Teuchos::ArrayRCP<const ST> colMask(gblCol->getData(0));
    const real_t eps = escript::DataTypes::real_t_eps();
    const ST zero = Teuchos::ScalarTraits<ST>::zero();

    //resumeFill();
// Can't use OpenMP here as replaceLocalValues() is not thread-safe.
//#pragma omp parallel for
    // loop through local row blocks
    for (LO lrb = 0; lrb < mat.getNodeNumRows(); lrb++) {
        LO numIndices = 0;
        const LO* indices;
        ST* values;
        mat.getLocalRowView(lrb, indices, values, numIndices);
        std::vector<GO> cols(numIndices);
        std::vector<ST> vals(numIndices*blockSize*blockSize);
        const GO rowblk = mat.getRowMap()->getGlobalElement(lrb);
        for (LO c = 0; c < numIndices; c++) {
            // local/global column block
            const LO lcb = indices[c];
            const GO colblk = mat.getColMap()->getGlobalElement(lcb);
            cols[c] = lcb;
            for (LO ri = 0; ri < blockSize; ri++) {
                const LO lclrow = lrb * blockSize + ri;
                const GO row = rowblk * blockSize + ri;
                for (LO ci = 0; ci < blockSize; ci++) {
                    const LO lclcol = lcb * blockSize + ci;
                    const GO col = colblk * blockSize + ci;
                    const size_t idx = INDEX3(ci, ri, c, blockSize, blockSize);
                    if (std::abs(rowMask[lclrow]) > eps || std::abs(colMask[lclcol]) > eps) {
                        vals[idx] = (row==col ? mdv : zero);
                    } else {
                        // we need to add full blocks so add current value
                        vals[idx] = values[idx];
                    }
                }
            }
        }
        mat.replaceLocalValues(lrb, &cols[0], &vals[0], numIndices);
    }
    //fillComplete(true);
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::saveMM(const std::string& filename) const
{
    Teuchos::ParameterList params;
    // for compatibility with paso, not strictly required.
    params.set("precision", 15);
    std::ofstream os(filename);
    Tpetra::Experimental::blockCrsMatrixWriter<ST,LO,GO,NT>(mat, os, params);
    os.close();
}

template<typename ST>
void BlockCrsMatrixWrapper<ST>::resetValues()
{
    //resumeFill();
    mat.setAllToScalar(static_cast<const ST>(0.));
    //fillComplete(true);
}

// instantiate
template class BlockCrsMatrixWrapper<real_t>;
template class BlockCrsMatrixWrapper<cplx_t>;

}  // end of namespace

