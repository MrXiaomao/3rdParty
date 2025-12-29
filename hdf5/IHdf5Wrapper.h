#ifndef IHDF5WRAPPER_H_
#define IHDF5WRAPPER_H_

#include <string>
#include <ctime>
#include <vector>
#include <hdf5.h>
#include <H5Fpublic.h>
using namespace std;

//HDF5写功能
class Hdf5WriteValue
{
public:
    // hid_t file, dataset, dataspace, group;
    // herr_t status;
	hid_t dtype = H5Tcopy(H5T_C_S1);
	
	Hdf5WriteValue(){};
	~Hdf5WriteValue(){};
    hid_t CreateNewFile(string fileName);
    hid_t CreateStringDataspace(const void *data, int rank, int row, int col);
    hid_t CreateDataspace(int rank, int row, int col);
    hid_t CreateGroup(const hid_t file, string groupName);
    hid_t CreateIntDataset(const hid_t group, const hid_t dataspace, string datasetName);
    hid_t CreateDoubleDataset(const hid_t group, const hid_t dataspace, string datasetName);
    hid_t CreateStringDataset(const hid_t group, const hid_t dataspace, string datasetName);
    herr_t WriteIntValue(const hid_t dataset, const void *data);
    herr_t WriteDoubleValue(const hid_t dataset, const void *data);
    herr_t WriteStringValue(const hid_t dataset, const void *data);
    herr_t CloseFile(const hid_t file, const hid_t group, const hid_t dataspace, const hid_t dataset);
};

//HDF5读功能
class Hdf5ReadValue
{
public:
	hid_t file, dataset, dataspace, group;
	herr_t status;
	hid_t dtype = H5Tcopy(H5T_C_S1);

    Hdf5ReadValue(){};
    ~Hdf5ReadValue(){};
	void OpenFile(const char* filepath);
	void OpenGroup(const char* group);
	void OpenDataset(const char* dataset);
	void ReadIntData(void *data);
	void ReadDoubleData(void *data);
	void ReadStringData(void *data);
	void CloseFile();
};

class QHDF5Group;
class QHDF5File{
public:
    QHDF5File();
    QHDF5File(const string &name);
    ~QHDF5File();

    /*打开文件*/
    bool open(unsigned flags = H5F_ACC_RDONLY);
    bool create(unsigned flags = H5F_ACC_TRUNC);

    /*关闭文件*/
    void close();

    /*创建组*/
    QHDF5Group* createGroup(const string &name);

    /*文件ID*/
    operator hid_t() const;
private:
    hid_t mFile;
    string mFileName;    
    std::vector<std::pair<std::string, QHDF5Group*>> mGroups;
};

class QHDF5Group{
public:
    QHDF5Group(const hid_t &file, const string &name);
    ~QHDF5Group();

    /*申请维度空间*/
    static hid_t createDataSpace(int rank, int row, int col);//2维
    static hid_t createStringDataSpace(const void *data, int rank = 1, int row = 1, int col = 1);//1维

    /*字节*/
    hid_t createCharDataset(const hid_t dataspace, string datasetName);
    /*整型*/
    hid_t createIntDataset(const hid_t dataspace, string datasetName);
    /*浮点型*/
    hid_t createDoubleDataset(const hid_t dataspace, string datasetName);
    /*字符串*/
    hid_t createStringDataset(const hid_t dataspace, string datasetName);

    /*整型*/
    herr_t writeIntValue(const hid_t dataset, const void *data);
    /*浮点型*/
    herr_t writeDoubleValue(const hid_t dataset, const void *data);
    /*字符串*/
    herr_t writeStringValue(const hid_t dataset, const void *data);

    /*组ID*/
    operator hid_t() const;
private:
    hid_t mGroup;
    string mGroupName;
    std::vector<hid_t> mDataspaces;
    std::vector<std::pair<std::string, hid_t>> mDatasets;
};

#endif
