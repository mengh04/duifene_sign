#include <cpr/cpr.h>
#include <duifene_sign.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <regex>

namespace duifene_sign {

using json = nlohmann::json;

class Session::Impl {
public:
    const std::string host = "https://www.duifene.com";
    cpr::Session session{};
    std::string stu_id;
    std::vector<CourseInfo> course_list{};
    // SignInfo sign_info{};

    std::string extract_code(const std::string& user_link);
    void get_user_id();
    void get_course_list();
    std::string extract_value_by_id(const std::string& html,
                                    const std::string& id,
                                    const std::string& attr_name);
    bool is_login();
    StudentAmount get_student_amount(const std::string& hf_check_in_id);
    void sign_code(const std::string& hf_check_code_key);
    void sign_qr(const std::string& hf_check_in_id);
    void sign_location(const std::string& hf_room_longitude,
                       const std::string& hf_room_latitude);
};

Session::Session() : impl(std::make_unique<Impl>()) {}
Session::~Session() = default;

void Session::login(const std::string& user_link) {
    const std::string code = impl->extract_code(user_link);
    auto url =
        cpr::Url{impl->host + "/P.aspx?authtype=1&code=" + code + "&state=1"};
    impl->session.SetUserAgent(cpr::UserAgent(
        "Mozilla/5.0 (iPhone; CPU iPhone OS 16_6 like Mac OS X) "
        "AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 "
        "MicroMessenger/8.0.40(0x1800282a) NetType/WIFI Language/zh_CN"));
    impl->session.SetUrl(url);
    const cpr::Response response = impl->session.Get();
    if (response.status_code == 200) {
        std::cout << "登录成功" << std::endl;
        impl->get_course_list();
    } else {
        std::cout << response.status_code << std::endl;
    }
}

void Session::Impl::get_course_list() {
    cpr::Header headers = {
        {"Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"},
        {"Referer",
         "https://www.duifene.com/_UserCenter/PC/CenterStudent.aspx"}};
    const std::string params = "action=getstudentcourse&classtypeid=2";
    session.SetHeader(headers);
    session.SetBody(params);
    session.SetUrl(host + "/_UserCenter/CourseInfo.ashx");
    const cpr::Response response = session.Get();
    if (response.status_code == 200) {
        json j = json::parse(response.text);
        for (auto& course : j) {
            course_list.emplace_back(course["CourseID"], course["CourseName"],
                                     course["TClassID"]);
        }
    } else {
        std::cout << response.status_code << std::endl;
    }
}

void Session::Impl::get_user_id() {
    session.SetUrl(host + "/_UserCenter/MB/index.aspx");
    cpr::Response response = session.Get();
    if (response.status_code == 200) {
        stu_id = extract_value_by_id(response.text, "hidUID", "value");
        std::cerr << stu_id << std::endl;
    } else {
        std::cout << response.status_code << std::endl;
    }
}

std::string Session::Impl::extract_value_by_id(const std::string& html,
                                               const std::string& id,
                                               const std::string& attr_name) {
    htmlDocPtr doc = htmlReadDoc(
        (const xmlChar*)html.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) {
        std::cerr << "Failed to parse HTML" << std::endl;
        return "";
    }

    // 2. 创建 XPath 上下文
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx) {
        std::cerr << "Failed to create XPath context" << std::endl;
        xmlFreeDoc(doc);
        return "";
    }

    // 3. 执行 XPath 查询：查找 id="hidUID" 的元素
    std::string xpath = "//*[@id='" + id + "']";
    xmlXPathObjectPtr result = xmlXPathEval((const xmlChar*)xpath.c_str(), ctx);
    if (!result || !result->nodesetval || result->nodesetval->nodeNr == 0) {
        std::cerr << "Element with id='" << id << "' not found" << std::endl;
        xmlXPathFreeObject(result);
        xmlXPathFreeContext(ctx);
        xmlFreeDoc(doc);
        return "";
    }

    // 4. 获取目标属性值
    xmlNodePtr node = result->nodesetval->nodeTab[0];
    xmlChar* attr_value = xmlGetProp(node, (const xmlChar*)attr_name.c_str());
    std::string value;
    if (attr_value) {
        value = (const char*)attr_value;
        xmlFree(attr_value);
    } else {
        std::cerr << "Attribute '" << attr_name << "' not found" << std::endl;
    }

    // 5. 释放资源
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);

    return value;
}

std::string Session::Impl::extract_code(const std::string& user_link) {
    std::regex pattern(R"(code=(\S{32}))");
    std::smatch match;
    std::regex_search(user_link, match, pattern);
    if (match.size() > 1) {
        return match[1].str();
    } else {
        return "";
    }
}

SignInfo Session::get_sign_info(const int idx) {
    SignInfo sign_info{};

    impl->session.SetHeader(
        {{"Referer", "https://www.duifene.com/_UserCenter/MB/index.aspx"}});
    impl->session.SetUrl(impl->host + "/_UserCenter/MB/Module.aspx?data=" +
                         impl->course_list.at(idx).course_id);
    impl->session.Get();

    if (!impl->is_login()) return sign_info;

    impl->session.SetUrl(
        impl->host + "/_checkin/mb/teachcheckin.aspx?classid=" +
        impl->course_list.at(idx).class_id +
        "&temps=0&checktype=1&isrefresh=0&timeinterval=0&roomid=0&match=");
    impl->session.UpdateHeader(
        {{"Referer", "https://www.duifene.com/_UserCenter/MB/index.aspx"}});
    cpr::Response response = impl->session.Get();
    std::cerr << 2 << std::endl;
    if (response.status_code == 200) {
        if (response.text.find("HFChecktype") != std::string::npos) {
            sign_info.hf_checktype = impl->extract_value_by_id(
                response.text, "HFChecktype", "value");
            sign_info.hf_check_in_id = impl->extract_value_by_id(
                response.text, "HFCheckInID", "value");
            sign_info.hf_class_id =
                impl->extract_value_by_id(response.text, "HFClassID", "value");
            sign_info.hf_seconds =
                impl->extract_value_by_id(response.text, "HFSeconds", "value");
        } else {
            std::cerr << "Not Found!" << std::endl;
        }
    } else {
        std::cout << response.status_code << std::endl;
    }
    if (sign_info.hf_checktype == "1") {
        sign_info.hf_check_code_key =
            impl->extract_value_by_id(response.text, "HFCheckCodeKey", "value");
    } else if (sign_info.hf_checktype == "3") {
        sign_info.hf_room_longitude = impl->extract_value_by_id(
            response.text, "HFRoomLongitude", "value");
        sign_info.hf_room_latitude =
            impl->extract_value_by_id(response.text, "HFRoomLatitude", "value");
    }
    sign_info.student_amount =
        impl->get_student_amount(sign_info.hf_check_in_id);
    return sign_info;
}

CourseInfo Session::get_course_info(const int idx) {
    if (idx < 0 || idx >= impl->course_list.size()) {
        std::cerr << "Index out of range" << std::endl;
        return CourseInfo{};
    }
    return impl->course_list.at(idx);
}

int Session::get_course_count() const {
    return static_cast<int>(impl->course_list.size());
}

bool Session::Impl::is_login() {
    cpr::Header headers = {
        {"Referer",
         "https://www.duifene.com/_UserCenter/PC/CenterStudent.aspx"},
        {"Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"}};
    session.SetPayload({{"Action", "checklogin"}});
    session.SetUrl(host + "/AppCode/LoginInfo.ashx");
    cpr::Response response = session.Get();
    if (response.status_code == 200) {
        json j = json::parse(response.text);
        if (j["msg"] == "1") {
            return true;
        } else {
            std::cerr << response.text << std::endl;
            return false;
        }
    } else {
        std::cerr << response.text << std::endl;
        return false;
    }
}

StudentAmount Session::Impl::get_student_amount(
    const std::string& hf_check_in_id) {
    StudentAmount student_amount{};
    cpr::Parameters params = {{"action", "getcheckintotalbyciid"},
                              {"ciid", hf_check_in_id},
                              {"t", "cking"}};
    session.SetUrl(host + "/_CheckIn/MBCount.ashx");
    session.SetParameters(params);

    cpr::Response response = session.Get();
    if (response.status_code == 200) {
        json j = json::parse(response.text);
        student_amount.signed_amount = static_cast<int>(j["TotalNumber"]) -
                                       static_cast<int>(j["AbsenceNumber"]);
        student_amount.total_amount = static_cast<int>(j["TotalNumber"]);
    } else {
        std::cerr << "get_student_amount error" << std::endl;
    }
    session.SetParameters({});

    return student_amount;
}

void Session::Impl::sign_code(const std::string& hf_check_code_key) {
    cpr::Payload payload = {{"action", "studentcheckin"},
                            {"studentid", stu_id},
                            {"checkincode", hf_check_code_key}};
    session.SetPayload(payload);
    session.SetHeader(
        {{"Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"},
         {"Referer",
          "https://www.duifene.com/_CheckIn/MB/"
          "CheckInStudent.aspx?moduleid=16&pasd="}});
    session.SetUrl(host + "/_CheckIn/CheckIn.ashx");
    cpr::Response response = session.Post();
    if (response.status_code == 200) {
        json j = json::parse(response.text);
        std::string msg = j["msgbox"];
        std::cout << msg << std::endl;
        std::cout << "签到码签到" << std::endl;
    }
}

void Session::Impl::sign_qr(const std::string& hf_check_in_id) {
    session.SetUrl(host +
                   "/_CheckIn/MB/QrCodeCheckOK.aspx?state=" + hf_check_in_id);
    cpr::Response response = session.Get();
    if (response.status_code == 200) {
        std::cout << "二维码签到" << std::endl;
    } else {
        std::cerr << "error" << std::endl;
    }
}

double random_offset(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::string format_coordinate(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8) << value;
    return oss.str();
}

void Session::Impl::sign_location(const std::string& hf_room_longitude,
                                  const std::string& hf_room_latitude) {
    double longitude =
        std::stod(hf_room_longitude) + random_offset(-0.000089, 0.000089);
    double latitude =
        std::stod(hf_room_latitude) + random_offset(-0.000089, 0.000089);

    std::string adjusted_longitude = format_coordinate(longitude);
    std::string adjusted_latitude = format_coordinate(latitude);

    session.SetHeader(
        {{"Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"},
         {"Referer",
          "https://www.duifene.com/_CheckIn/MB/"
          "CheckInStudent.aspx?moduleid=16&pasd="}});
    session.SetParameters({{"action", "signin"},
                           {"sid", stu_id},
                           {"longitude", adjusted_longitude},
                           {"latitude", adjusted_latitude}});
    session.SetUrl(host + "/_CheckIn/CheckInRoomHandler.ashx");
    cpr::Response response = session.Post();
    if (response.status_code == 200) {
        std::cout << "定位签到" << std::endl;
    } else {
        std::cerr << "error" << std::endl;
    }
}

void Session::sign(const SignInfo& sign_info) {
    impl->get_user_id();
    if (sign_info.hf_checktype == "1") {
        impl->sign_code(sign_info.hf_check_code_key);
    } else if (sign_info.hf_checktype == "2") {
        impl->sign_qr(sign_info.hf_check_in_id);
    } else if (sign_info.hf_checktype == "3") {
        impl->sign_location(sign_info.hf_room_longitude,
                            sign_info.hf_room_latitude);
    }
}

}  // namespace duifene_sign