#include <duifene_sign_c.h>
#include <cstdio>

int main() {
    SessionHandle session = create_session();
    char user_link[256];
    printf("请输入登录链接: ");
    scanf("%s", user_link);
    session_login(session, user_link);
    
    for (int i = 0; i < get_course_count(session); ++i) {
        CourseInfo_C course_info = get_course_info(session, i);
        printf("%d: %s\n", i, course_info.course_name);
    }

    int idx;
 
    while (scanf("%d", &idx) == 1) {
        SignInfo_C sign_info = get_sign_info(session, idx);
        if (sign_info.hf_checktype[0] != '\0') {
            do_sign(session, sign_info);
        }
    }

    destroy_session(session);
}