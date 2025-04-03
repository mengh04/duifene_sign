#include "duifene_sign_c.h"
#include "duifene_sign.h"
using namespace duifene_sign;

extern "C" {

SessionHandle create_session() {
    return new Session();
}

void destroy_session(SessionHandle handle) {
    delete static_cast<Session*>(handle);
}

void session_login(SessionHandle handle, const char* user_link) {
    static_cast<Session*>(handle)->login(user_link);
}

SignInfo_C get_sign_info(SessionHandle handle, int index) {
    auto info = static_cast<Session*>(handle)->get_sign_info(index);
    SignInfo_C c_info;

    // 深拷贝字符串（确保不越界并手动终止）
    strncpy((char*)c_info.hf_seconds, info.hf_seconds.c_str(), sizeof(c_info.hf_seconds) - 1);
    c_info.hf_seconds[sizeof(c_info.hf_seconds) - 1] = '\0';

    strncpy((char*)c_info.hf_check_code_key, info.hf_check_code_key.c_str(), sizeof(c_info.hf_check_code_key) - 1);
    c_info.hf_check_code_key[sizeof(c_info.hf_check_code_key) - 1] = '\0';

    strncpy((char*)c_info.hf_checktype, info.hf_checktype.c_str(), sizeof(c_info.hf_checktype) - 1);
    c_info.hf_checktype[sizeof(c_info.hf_checktype) - 1] = '\0';

    // 调试输出（深拷贝后安全）

    // 其他字段同理
    strncpy((char*)c_info.hf_check_in_id, info.hf_check_in_id.c_str(), sizeof(c_info.hf_check_in_id) - 1);
    c_info.hf_check_in_id[sizeof(c_info.hf_check_in_id) - 1] = '\0';

    strncpy((char*)c_info.hf_class_id, info.hf_class_id.c_str(), sizeof(c_info.hf_class_id) - 1);
    c_info.hf_class_id[sizeof(c_info.hf_class_id) - 1] = '\0';

    strncpy((char*)c_info.hf_room_longitude, info.hf_room_longitude.c_str(), sizeof(c_info.hf_room_longitude) - 1);
    c_info.hf_room_longitude[sizeof(c_info.hf_room_longitude) - 1] = '\0';

    strncpy((char*)c_info.hf_room_latitude, info.hf_room_latitude.c_str(), sizeof(c_info.hf_room_latitude) - 1);
    c_info.hf_room_latitude[sizeof(c_info.hf_room_latitude) - 1] = '\0';

    // 拷贝数值
    c_info.student_amount.total_amount = info.student_amount.total_amount;
    c_info.student_amount.signed_amount = info.student_amount.signed_amount;

    return c_info;
}

CourseInfo_C get_course_info(SessionHandle handle, int index) {
    auto info = static_cast<Session*>(handle)->get_course_info(index);
    CourseInfo_C c_info;

    // 深拷贝字符串（确保不越界并手动终止）
    strncpy((char*)c_info.course_id, info.course_id.c_str(), sizeof(c_info.course_id) - 1);
    c_info.course_id[sizeof(c_info.course_id) - 1] = '\0';

    strncpy((char*)c_info.course_name, info.course_name.c_str(), sizeof(c_info.course_name) - 1);
    c_info.course_name[sizeof(c_info.course_name) - 1] = '\0';

    strncpy((char*)c_info.class_id, info.class_id.c_str(), sizeof(c_info.class_id) - 1);
    c_info.class_id[sizeof(c_info.class_id) - 1] = '\0';

    return c_info;
}

int get_course_count(SessionHandle handle) {
    return static_cast<Session*>(handle)->get_course_count();
}

void do_sign(SessionHandle handle, SignInfo_C sign_info) {
    SignInfo info;
    // 转换C结构体到C++结构体
    info.hf_seconds = sign_info.hf_seconds;
    info.hf_checktype = sign_info.hf_checktype;
    info.hf_check_in_id = sign_info.hf_check_in_id;
    info.hf_class_id = sign_info.hf_class_id;
    info.hf_check_code_key = sign_info.hf_check_code_key;
    info.hf_room_longitude = sign_info.hf_room_longitude;
    info.hf_room_latitude = sign_info.hf_room_latitude;
    info.student_amount.total_amount = sign_info.student_amount.total_amount;
    info.student_amount.signed_amount = sign_info.student_amount.signed_amount;
    
    static_cast<Session*>(handle)->sign(info);
}

} // extern "C"