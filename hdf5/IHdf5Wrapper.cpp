#include "IHdf5Wrapper.h"
using namespace std;

QHDF5File::QHDF5File():
    mFile(0){

}

QHDF5File::QHDF5File(const string &name):
    mFile(0),
    mFileName(name){

}

QHDF5File::~QHDF5File(){

}

/*打开文件*/
bool QHDF5File::open(unsigned flags/* = H5F_ACC_RDONLY*/)
{
    if (mFileName.empty())
        return false;

    mFile = H5Fopen(mFileName.c_str(), flags, H5P_DEFAULT);
    if (mFile < 0)
        return false;
    else
        return true;
}

bool QHDF5File::create(unsigned flags/* = H5F_ACC_TRUNC*/)
{
    if (mFileName.empty())
        return false;

    mFile = H5Fcreate(mFileName.c_str(), flags, H5P_DEFAULT, H5P_DEFAULT);
    if (mFile < 0)
        return false;
    else
        return true;
}

/*关闭文件*/
void QHDF5File::close()
{
    for (int i=0; i<mGroups.size(); ++i){
        delete mGroups[i].second;
    }
    H5Fclose(mFile);
}

/*创建组*/

QHDF5Group* QHDF5File::createGroup(const string &name)
{
    QHDF5Group* group = new QHDF5Group(this->mFile, name);
    mGroups.push_back(std::make_pair(name, group));
    return group;
}

QHDF5File::operator hid_t() const
{
    return this->mFile;
}

//////////////////////////////////////////////////////////////////////////////////////
QHDF5Group::QHDF5Group(const hid_t &file, const string &name)
{
    this->mGroup = H5Gcreate(file, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}

QHDF5Group::~QHDF5Group()
{
    for (int i=0; i<mDatasets.size(); ++i){
        H5Gclose(mDatasets[i].second);
    }

    for (int i=0; i<mDataspaces.size(); ++i){
        H5Gclose(mDataspaces[i]);
    }
    H5Gclose(mGroup);
}

/*申请维度空间*/
hid_t QHDF5Group::createDataSpace(int rank, int row, int col)
{
    hsize_t dim[2];
    dim[0] = row;
    dim[1] = col;
    hid_t dataspace = H5Screate_simple(rank, dim, NULL);
    return dataspace;
}

hid_t QHDF5Group::createStringDataSpace(const void *data, int rank, int row, int col)
{
    hsize_t dim[2];
    dim[0] = row;
    dim[1] = col;
    size_t size = sizeof(data) / sizeof(char);
    herr_t status = H5Tset_size(H5Tcopy(H5T_C_S1), size);
    if (status<0){
        /*失败了*/
        return -1;
    }

    hid_t dataspace = H5Screate_simple(rank, dim, NULL);
    return dataspace;
}

/*字节*/
hid_t QHDF5Group::createCharDataset(const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(this->mGroup, datasetName.c_str(), H5T_STD_I8LE, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    mDatasets.push_back(std::make_pair(datasetName, dataset));
    return dataset;
}

hid_t QHDF5Group::createIntDataset(const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(this->mGroup, datasetName.c_str(), H5T_NATIVE_INT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    mDatasets.push_back(std::make_pair(datasetName, dataset));
    return dataset;
}

hid_t QHDF5Group::createDoubleDataset(const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(this->mGroup, datasetName.c_str(), H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return dataset;
}

hid_t QHDF5Group::createStringDataset(const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(this->mGroup, datasetName.c_str(), H5Tcopy(H5T_C_S1), dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return dataset;
}

/*写入数据到对应类型的dataset*/
herr_t QHDF5Group::writeIntValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

herr_t QHDF5Group::writeDoubleValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

herr_t QHDF5Group::writeStringValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, H5Tcopy(H5T_C_S1), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

QHDF5Group::operator hid_t() const
{
    return this->mGroup;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////
