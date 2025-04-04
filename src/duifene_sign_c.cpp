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

    c_info.hf_seconds = strdup(info.hf_seconds.c_str());
    c_info.hf_checktype = strdup(info.hf_checktype.c_str());
    c_info.hf_check_in_id = strdup(info.hf_check_in_id.c_str());
    c_info.hf_class_id = strdup(info.hf_class_id.c_str());
    c_info.hf_check_code_key = strdup(info.hf_check_code_key.c_str());
    c_info.hf_room_longitude = strdup(info.hf_room_longitude.c_str());
    c_info.hf_room_latitude = strdup(info.hf_room_latitude.c_str());
    c_info.student_amount.total_amount = info.student_amount.total_amount;
    c_info.student_amount.signed_amount = info.student_amount.signed_amount;

    return c_info;
}

void free_sign_info(SignInfo_C sign_info) {
    free(sign_info.hf_seconds);
    free(sign_info.hf_checktype);
    free(sign_info.hf_check_in_id);
    free(sign_info.hf_class_id);
    free(sign_info.hf_check_code_key);
    free(sign_info.hf_room_longitude);
    free(sign_info.hf_room_latitude);
}

CourseInfo_C get_course_info(SessionHandle handle, int index) {
    auto info = static_cast<Session*>(handle)->get_course_info(index);
    CourseInfo_C c_info;

    c_info.course_id = strdup(info.course_id.c_str());
    c_info.course_name = strdup(info.course_name.c_str());
    c_info.class_id = strdup(info.class_id.c_str());

    return c_info;
}

void free_course_info(CourseInfo_C course_info) {
    free(course_info.course_id);
    free(course_info.course_name);
    free(course_info.class_id);
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