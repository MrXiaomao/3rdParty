#include "IHdf5Wrapper.h"
using namespace std;

/*创建HDF5*/
hid_t Hdf5WriteValue::CreateNewFile(string fileName)
{
    hid_t file = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	//H5F_ACC_TRUNC能覆盖，H5F_ACC_EXCL不能覆盖
    return file;
}

/*创建HDF5 String数据维度*/
hid_t Hdf5WriteValue::CreateStringDataspace(const void *data, int rank, int row, int col)
{
	hsize_t dim[2];
	dim[0] = row;
	dim[1] = col;
	size_t size = sizeof(data) / sizeof(char);
    herr_t status = H5Tset_size(dtype, size);
    if (status<0){
        /*失败了*/
        return -1;
    }

    hid_t dataspace = H5Screate_simple(rank, dim, NULL);
    return dataspace;
}

/*创建HDF5数据维度*/
hid_t Hdf5WriteValue::CreateDataspace(int rank, int row, int col)
{
	hsize_t dim[2];
	dim[0] = row;
	dim[1] = col;
    hid_t dataspace = H5Screate_simple(rank, dim, NULL);
    return dataspace;
}

/*创建group*/
hid_t Hdf5WriteValue::CreateGroup(const hid_t file, string groupName)
{
    hid_t group = H5Gcreate(file, groupName.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return group;
}

/*创建dataset,int,double,string*/
hid_t Hdf5WriteValue::CreateIntDataset(const hid_t group, const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(group, datasetName.c_str(), H5T_NATIVE_INT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return dataset;
}

hid_t Hdf5WriteValue::CreateDoubleDataset(const hid_t group, const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(group, datasetName.c_str(), H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return dataset;
}

hid_t Hdf5WriteValue::CreateStringDataset(const hid_t group, const hid_t dataspace, string datasetName)
{
    hid_t dataset = H5Dcreate2(group, datasetName.c_str(), this->dtype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    return dataset;
}

/*写入数据到对应类型的dataset*/
herr_t Hdf5WriteValue::WriteIntValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

herr_t Hdf5WriteValue::WriteDoubleValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

herr_t Hdf5WriteValue::WriteStringValue(const hid_t dataset, const void *data)
{
    herr_t status = H5Dwrite(dataset, this->dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    return status;
}

/*关闭数据及文件，注意关闭顺序*/
herr_t Hdf5WriteValue::CloseFile(const hid_t file, const hid_t group, const hid_t dataspace, const hid_t dataset)
{
    herr_t status = H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Gclose(group);
    H5Fclose(file);
    return status;
}
