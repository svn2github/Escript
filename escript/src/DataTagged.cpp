// $Id$

/*
 ******************************************************************************
 *                                                                            *
 *       COPYRIGHT  ACcESS 2004 -  All Rights Reserved                        *
 *                                                                            *
 * This software is the property of ACcESS. No part of this code              *
 * may be copied in any form or by any means without the expressed written    *
 * consent of ACcESS.  Copying, use or modification of this software          *
 * by any unauthorised person is illegal unless that person has a software    *
 * license agreement with ACcESS.                                             *
 *                                                                            *
 ******************************************************************************
*/

#include "DataTagged.h"

#include "DataConstant.h"
#include "DataException.h"

using namespace std;

namespace escript {

DataTagged::DataTagged()
  : DataAbstract(FunctionSpace())
{
  // default constructor

  // create a scalar default value
  m_data.resize(1,0.,1);
  DataArrayView temp(m_data,DataArrayView::ShapeType());
  setPointDataView(temp);
}

DataTagged::DataTagged(const TagListType& tagKeys, 
		       const ValueListType& values,
		       const DataArrayView& defaultValue,
		       const FunctionSpace& what)
  : DataAbstract(what)
{
  // constructor

  // initialise the array of data values
  // the default value is always the first item in the values list
  int len = defaultValue.noValues();
  m_data.resize(len,0.,len);
  for (int i=0; i<defaultValue.noValues(); i++) {
    m_data[i]=defaultValue.getData(i);
  }

  // create the data view
  DataArrayView temp(m_data,defaultValue.getShape());
  setPointDataView(temp);

  // add remaining tags and values
  addTaggedValues(tagKeys,values);
}

DataTagged::DataTagged(const FunctionSpace& what,
                       const DataArrayView::ShapeType &shape,
                       const int tags[],
                       const ValueType& data)
  : DataAbstract(what)
{
  // alternative constructor
  // not unit_tested tested yet

  // copy the data
  m_data=data;

  // create the view of the data
  DataArrayView tempView(m_data,shape);
  setPointDataView(tempView);

  // create the tag lookup map
  for (int sampleNo=0; sampleNo<getNumSamples(); sampleNo++) {
    m_offsetLookup.insert(DataMapType::value_type(sampleNo,tags[sampleNo]));
  }
}

DataTagged::DataTagged(const DataTagged& other)
  : DataAbstract(other.getFunctionSpace()),
  m_data(other.m_data),
  m_offsetLookup(other.m_offsetLookup)
{
  // copy constructor

  // create the data view
  DataArrayView temp(m_data,other.getPointDataView().getShape());
  setPointDataView(temp);
}

DataTagged::DataTagged(const DataConstant& other)
  : DataAbstract(other.getFunctionSpace())
{
  // copy constructor

  // fill the default value with the constant value item from "other"
  const DataArrayView& value=other.getPointDataView();
  int len = value.noValues();
  m_data.resize(len,0.,len);
  for (int i=0; i<value.noValues(); i++) {
    m_data[i]=value.getData(i);
  }

  // create the data view
  DataArrayView temp(m_data,value.getShape());
  setPointDataView(temp);
}

void
DataTagged::reshapeDataPoint(const DataArrayView::ShapeType& shape) 
{
  // can only reshape a rank zero data point
  if (getPointDataView().getRank()!=0) {
    stringstream temp;
    temp << "Error - Can only reshape Data with data points of rank 0. "
	 << "This Data has data points with rank: " 
	 << getPointDataView().getRank();
    throw DataException(temp.str());
  }

  // allocate enough space for all values
  DataArrayView::ValueType newData(DataArrayView::noValues(shape)*(m_offsetLookup.size()+1));
  DataArrayView newView(newData,shape);
  newView.copy(0,getDefaultValue()());

  // loop through the tag values
  DataMapType::iterator pos;
  DataArrayView::ValueType::size_type tagOffset=DataArrayView::noValues(shape);
  for (pos=m_offsetLookup.begin();pos!=m_offsetLookup.end();pos++){
    newView.copy(tagOffset,m_data[pos->second]);
    pos->second=tagOffset;
    tagOffset+=DataArrayView::noValues(shape);
  }
  m_data=newData;
  DataArrayView temp(m_data,shape);
  setPointDataView(temp);
}

DataAbstract*
DataTagged::getSlice(const DataArrayView::RegionType& region) const 
{
  return new DataTagged(*this, region);
}

DataTagged::DataTagged(const DataTagged& other, 
		       const DataArrayView::RegionType& region)
  : DataAbstract(other.getFunctionSpace())
{
  // slice constructor

  // get the shape of the slice to copy from other
  DataArrayView::ShapeType regionShape(DataArrayView::getResultSliceShape(region));
  DataArrayView::RegionLoopRangeType regionLoopRange=getSliceRegionLoopRange(region);

  // allocate enough space in this for all values
  // (need to add one to allow for the default value)
  int len = DataArrayView::noValues(regionShape)*(other.m_offsetLookup.size()+1);
  m_data.resize(len,0.0,len);

  // create the data view
  DataArrayView temp(m_data,regionShape);
  setPointDataView(temp);

  // copy the default value from other to this
  getDefaultValue().copySlice(other.getDefaultValue(), regionLoopRange);

  // loop through the tag values copying these
  DataMapType::const_iterator pos;
  DataArrayView::ValueType::size_type tagOffset=getPointDataView().noValues();
  for (pos=other.m_offsetLookup.begin();pos!=other.m_offsetLookup.end();pos++){
    getPointDataView().copySlice(tagOffset,other.getPointDataView(),pos->second,regionLoopRange);
    m_offsetLookup.insert(DataMapType::value_type(pos->first,tagOffset));
    tagOffset+=getPointDataView().noValues();
  }
}

void
DataTagged::setSlice(const DataAbstract* other,
                     const DataArrayView::RegionType& region)
{

  // other must be another DataTagged object
  // Data:setSlice implementation should ensure this
  const DataTagged* otherTemp=dynamic_cast<const DataTagged*>(other);
  if (otherTemp==0) {
    throw DataException("Programming error - casting to DataTagged.");
  }

  // determine shape of the specified region
  DataArrayView::ShapeType regionShape(DataArrayView::getResultSliceShape(region));

  // modify region specification as needed to match rank of this object
  DataArrayView::RegionLoopRangeType regionLoopRange=getSliceRegionLoopRange(region);

  // ensure rank/shape of this object is compatible with specified region
  if (getPointDataView().getRank()!=region.size()) {
    throw DataException("Error - Invalid slice region.");
  }
  if (otherTemp->getPointDataView().getRank()>0 and !other->getPointDataView().checkShape(regionShape)) {
    throw DataException (other->getPointDataView().createShapeErrorMessage(
                         "Error - Couldn't copy slice due to shape mismatch.",regionShape));
  }

  // copy slice from other default value to this default value
  getDefaultValue().copySliceFrom(otherTemp->getDefaultValue(), regionLoopRange);

  // loop through tag values in other, adding any which aren't in this, using default value
  DataMapType::const_iterator pos;
  for (pos=otherTemp->m_offsetLookup.begin();pos!=otherTemp->m_offsetLookup.end();pos++) {
    if (!isCurrentTag(pos->first)) {
      addTaggedValue(pos->first,getDefaultValue());
    }
  }

  // loop through the tag values copying slices from other to this
  for (pos=m_offsetLookup.begin();pos!=m_offsetLookup.end();pos++) {
    getDataPointByTag(pos->first).copySliceFrom(otherTemp->getDataPointByTag(pos->first), regionLoopRange);
  }

}

int
DataTagged::getTagNumber(int dpno)
{
  //
  // Get the number of samples and data-points per sample
  int numSamples = getNumSamples();
  int numDataPointsPerSample = getNumDPPSample();
  int numDataPoints = numSamples * numDataPointsPerSample;

  if (numDataPointsPerSample==0) {
    throw DataException("DataTagged::getTagNumber error: no data-points associated with this object.");
  }

  if (dpno<0 || dpno>numDataPoints-1) {
    throw DataException("DataTagged::getTagNumber error: invalid data-point number supplied.");
  }

  //
  // Determine the sample number which corresponds to this data-point number
  int sampleNo = dpno / numDataPointsPerSample;

  //
  // Determine the tag number which corresponds to this sample number
  int tagNo = getFunctionSpace().getTagFromSampleNo(sampleNo);

  //
  // return the tag number
  return(tagNo);
}

void
DataTagged::setTaggedValues(const TagListType& tagKeys,
                            const ValueListType& values)
{
  addTaggedValues(tagKeys,values);
}

void
DataTagged::setTaggedValue(int tagKey,
                           const DataArrayView& value)
{
  if (!getPointDataView().checkShape(value.getShape())) {
      throw DataException(getPointDataView().createShapeErrorMessage(
                          "Error - Cannot setTaggedValue due to shape mismatch.", value.getShape()));
  }
  DataMapType::iterator pos(m_offsetLookup.find(tagKey));
  if (pos==m_offsetLookup.end()) {
    // tag couldn't be found so use addTaggedValue
    addTaggedValue(tagKey,value);
  } else {
    // copy the values into the data array at the offset determined by m_offsetLookup
    int offset=pos->second;
    for (int i=0; i<getPointDataView().noValues(); i++) {
      m_data[offset+i]=value.getData(i);
    }
  }
}

void
DataTagged::addTaggedValues(const TagListType& tagKeys,
                            const ValueListType& values)
{
  if (values.size()==0) {
    // copy the current default value for each of the tags
    TagListType::const_iterator iT;
    for (iT=tagKeys.begin();iT!=tagKeys.end();iT++) {
      // the point data view for DataTagged points at the default value
      addTaggedValue(*iT,getPointDataView());
    }
  } else if (values.size()==1 && tagKeys.size()>1) {
    // assume the one given value will be used for all tag values
    TagListType::const_iterator iT;
    for (iT=tagKeys.begin();iT!=tagKeys.end();iT++) {
      addTaggedValue(*iT,values[0]);
    }
  } else {
    if (tagKeys.size()!=values.size()) {
      stringstream temp;
      temp << "Error - (addTaggedValue) Number of tags: " << tagKeys.size()
	   << " doesn't match number of values: " << values.size();
      throw DataException(temp.str());
    } else {
      for (int i=0;i<tagKeys.size();i++) {
        addTaggedValue(tagKeys[i],values[i]);
      }
    }
  }
}

void
DataTagged::addTaggedValue(int tagKey,
                           const DataArrayView& value)
{
  if (!getPointDataView().checkShape(value.getShape())) {
    throw DataException(getPointDataView().createShapeErrorMessage(
                        "Error - Cannot addTaggedValue due to shape mismatch.", value.getShape()));
  }
  DataMapType::iterator pos(m_offsetLookup.find(tagKey));
  if (pos!=m_offsetLookup.end()) {
    // tag already exists so use setTaggedValue
    setTaggedValue(tagKey,value);
  } else {
    // save the key and the location of its data in the lookup tab
    m_offsetLookup.insert(DataMapType::value_type(tagKey,m_data.size()));
    // add the data given in "value" at the end of m_data
    // need to make a temp copy of m_data, resize m_data, then copy
    // all the old values plus the value to be added back into m_data
    ValueType m_data_temp(m_data);
    int oldSize=m_data.size();
    int newSize=m_data.size()+value.noValues();
    m_data.resize(newSize,0.,newSize);
    for (int i=0;i<oldSize;i++) {
      m_data[i]=m_data_temp[i];
    }
    for (int i=0;i<value.noValues();i++) {
      m_data[oldSize+i]=value.getData(i);
    }
  }
}

double*
DataTagged::getSampleDataByTag(int tag)
{
  DataMapType::iterator pos(m_offsetLookup.find(tag));
  if (pos==m_offsetLookup.end()) {
    // tag couldn't be found so return the default value
    return &(m_data[0]);
  } else {
    // return the data-point corresponding to the given tag
    return &(m_data[pos->second]);
  }
}

string
DataTagged::toString() const
{
  stringstream temp;
  DataMapType::const_iterator i;
  temp << "Tag(Default)" << endl;
  temp << getDefaultValue().toString() << endl;
  // create a temporary view as the offset will be changed
  DataArrayView tempView(getPointDataView().getData(), getPointDataView().getShape());
  for (i=m_offsetLookup.begin();i!=m_offsetLookup.end();++i) {
    temp << "Tag(" << i->first << ")" << endl;
    tempView.setOffset(i->second);
    temp << tempView.toString() << endl;
  }
  return temp.str();
}

DataArrayView::ValueType::size_type 
DataTagged::getPointOffset(int sampleNo,
                           int dataPointNo) const
{
  int tagKey=getFunctionSpace().getTagFromSampleNo(sampleNo);
  DataMapType::const_iterator pos(m_offsetLookup.find(tagKey));
  DataArrayView::ValueType::size_type offset=m_defaultValueOffset;
  if (pos!=m_offsetLookup.end()) {
    offset=pos->second;
  }
  return offset;
}

DataArrayView
DataTagged::getDataPointByTag(int tag) const
{
  DataMapType::const_iterator pos(m_offsetLookup.find(tag));
  DataArrayView::ValueType::size_type offset=m_defaultValueOffset;
  if (pos!=m_offsetLookup.end()) {
    offset=pos->second;
  }
  DataArrayView temp(getPointDataView());
  temp.setOffset(offset);
  return temp;
}

DataArrayView
DataTagged::getDataPoint(int sampleNo,
                         int dataPointNo)
{
  EsysAssert(validSampleNo(sampleNo),"(getDataPoint) Invalid sampleNo: " << sampleNo);
  int tagKey=getFunctionSpace().getTagFromSampleNo(sampleNo);
  return getDataPointByTag(tagKey);
}

int
DataTagged::archiveData(ofstream& archiveFile,
                        const DataArrayView::ValueType::size_type noValues) const
{
  return(m_data.archiveData(archiveFile, noValues));
}

int
DataTagged::extractData(ifstream& archiveFile,
                        const DataArrayView::ValueType::size_type noValues)
{
  return(m_data.extractData(archiveFile, noValues));
}
void
DataTagged::eigenvalues(DataAbstract* ev)
{
  DataTagged* temp_ev=dynamic_cast<DataTagged*>(ev);
  if (temp_ev==0) {
    throw DataException("Error - DataTagged::eigenvalues casting to DataTagged failed (propably a programming error).");
  }
  const DataTagged::DataMapType& thisLookup=getTagLookup();
  DataTagged::DataMapType::const_iterator i;
  DataTagged::DataMapType::const_iterator thisLookupEnd=thisLookup.end();
  for (i=thisLookup.begin();i!=thisLookupEnd;i++) {
      temp_ev->addTaggedValue(i->first,temp_ev->getDefaultValue());
      DataArrayView thisView=getDataPointByTag(i->first);
      DataArrayView evView=temp_ev->getDataPointByTag(i->first);
cout << i->first << thisView(0,0) << "\n";
      DataArrayView::eigenvalues(thisView,0,evView,0);
  }
  DataArrayView::eigenvalues(getDefaultValue(),0,temp_ev->getDefaultValue(),0);
}
void
DataTagged::eigenvalues_and_eigenvectors(DataAbstract* ev,DataAbstract* V,const double tol)
{
  DataTagged* temp_ev=dynamic_cast<DataTagged*>(ev);
  if (temp_ev==0) {
    throw DataException("Error - DataTagged::eigenvalues_and_eigenvectors casting to DataTagged failed (propably a programming error).");
  }
  DataTagged* temp_V=dynamic_cast<DataTagged*>(V);
  if (temp_V==0) {
    throw DataException("Error - DataTagged::eigenvalues_and_eigenvectors casting to DataTagged failed (propably a programming error).");
  }
  const DataTagged::DataMapType& thisLookup=getTagLookup();
  DataTagged::DataMapType::const_iterator i;
  DataTagged::DataMapType::const_iterator thisLookupEnd=thisLookup.end();
  for (i=thisLookup.begin();i!=thisLookupEnd;i++) {
      temp_ev->addTaggedValue(i->first,temp_ev->getDefaultValue());
      temp_V->addTaggedValue(i->first,temp_V->getDefaultValue());
      DataArrayView thisView=getDataPointByTag(i->first);
      DataArrayView evView=temp_ev->getDataPointByTag(i->first);
      DataArrayView VView=temp_V->getDataPointByTag(i->first);
      DataArrayView::eigenvalues_and_eigenvectors(thisView,0,evView,0,VView,0,tol);
  }
  DataArrayView::eigenvalues_and_eigenvectors(getDefaultValue(),0,
                                              temp_ev->getDefaultValue(),0,
                                              temp_V->getDefaultValue(),0,
                                              tol);


}


}  // end of namespace
