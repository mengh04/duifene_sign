#include <duifene_sign_c.h>
#include <cstdio>

int main() {
    SessionHandle session = create_session();
    char user_link[256];
    printf("请输入登录链接: ");
    scanf("%s", user_link);
    session_login(session, user_link);
    print_course_list(session);
    int idx;
 
    while (scanf("%d", &idx) == 1) {
        SignInfo_C sign_info = get_sign_info(session, idx);
        if (sign_info.hf_checktype[0] != '\0') {
            do_sign(session, sign_info);
        }
        print_sign_info(sign_info);
    }
    destroy_session(session);
}