#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// 定义课程结构体 (C 语言版本)
typedef struct {
    char course_id[16];
    char course_name[128];
    char class_id[16];
} CourseInfo_C;

// 定义学生签到数量结构体 (C 语言版本)
typedef struct {
    int total_amount;
    int signed_amount;
} StudentAmount_C;

// 定义签到信息结构体 (C 语言版本)
typedef struct {
    char hf_seconds[8];
    char hf_checktype[4];
    char hf_check_in_id[8];
    char hf_class_id[8];
    char hf_check_code_key[8];
    char hf_room_longitude[128];
    char hf_room_latitude[128];
    StudentAmount_C student_amount;
} SignInfo_C;

// 定义会话句柄 (不透明指针)
typedef void* SessionHandle;

// 创建新会话
SessionHandle create_session();

// 销毁会话
void destroy_session(SessionHandle handle);

// 登录
void session_login(SessionHandle handle, const char* user_link);

// 获取课程数量
int get_course_count(SessionHandle handle);

// 获取指定课程信息
CourseInfo_C get_course_info(SessionHandle handle, int index);

// 获取签到信息
SignInfo_C get_sign_info(SessionHandle handle, int index);

// 执行签到
void do_sign(SessionHandle handle, SignInfo_C sign_info);

#ifdef __cplusplus
}
#endif