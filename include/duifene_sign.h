#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace duifene_sign {

struct Course {
    std::string course_id;
    std::string course_name;
    std::string class_id;
};

struct StudentAmount {
    int total_amount, signed_amount;
};

struct SignInfo {
    std::string hf_seconds;
    std::string hf_checktype;
    std::string hf_check_in_id;
    std::string hf_class_id;
    std::string hf_check_code_key;
    std::string hf_room_longitude;
    std::string hf_room_latitude;
    StudentAmount student_amount;

    void print();
};

class Session {
    class Impl;
    std::unique_ptr<Impl> impl;
public:
    Session();
    ~Session();

    void login(const std::string& user_link);
    void print_course_list();
    void sign(const SignInfo& sign_info);
    SignInfo get_sign_info(const int idx);

};

}  // namespace duifene_sign
