#ifndef STATUS_RECORD_SERVICE_H
#define STATUS_RECORD_SERVICE_H

/**
 * 提交记录服务类头文件
 */
class StatusRecordService {
private:
    StatusRecordService();

    ~StatusRecordService();

public:
    // 局部静态特性的方式实现单实例模式
    static StatusRecordService *GetInstance();
};

#endif  // STATUS_RECORD_SERVICE_H