/**
 * 本文件提供 api.h 的 mock 实现, 仅输出日志并返回模拟数据, 可在任何平台编译.
 */

//#define pair {a, b} std::make_pair(a, b)

#include "../core/api.h"

#include <filesystem>
#include <iostream>

#include "../core/dir.h"
#include "../core/event.h"
#include "./mock.h"

namespace fs = std::experimental::filesystem;
using std::cout;
using std::endl;
using std::boolalpha;
using std::string;
using std::pair;

namespace cq {
    void _init_api() {
    }

#pragma region Message

    static int64_t curr_message_id = 0;

    template <typename... Args>
    void print_api_call(const string &name, const pair<const char *, Args> &... args) {
        cout << name << endl;
        ((cout << "  " << string(args.first) << ": " << boolalpha << to_string(args.second) << endl), ...);
    }

    int64_t send_private_message(const int64_t user_id, const std::string &message) {
        print_api_call("send_private_message", std::make_pair("user_id", user_id), std::make_pair("message", message));
        return ++curr_message_id;
    }

    int64_t send_group_message(const int64_t group_id, const std::string &message) {
        print_api_call("send_group_message", std::make_pair("group_id", group_id), std::make_pair("message", message));
        return ++curr_message_id;
    }

    int64_t send_discuss_message(const int64_t discuss_id, const std::string &message) {
        print_api_call("send_discuss_message", std::make_pair("discuss_id", discuss_id), std::make_pair("message", message));
        return ++curr_message_id;
    }

    void delete_message(const int64_t message_id) {
        print_api_call("delete_message", std::make_pair("message_id", message_id));
    }

#pragma endregion

#pragma region Friend Operation

    void send_like(const int64_t user_id, const int32_t times) {
        print_api_call("send_like", std::make_pair("user_id", user_id), std::make_pair("times", times));
    }

#pragma endregion

#pragma region Group Operation

    void set_group_kick(const int64_t group_id, const int64_t user_id, const bool reject_future_request) {
        print_api_call("set_group_kick",
                       std::make_pair("group_id", group_id),
                       std::make_pair("user_id", user_id),
                       std::make_pair("reject_future_request", reject_future_request));
    }

    void set_group_ban(const int64_t group_id, const int64_t user_id, const int64_t duration) {
        print_api_call(
            "set_group_ban", std::make_pair("group_id", group_id), std::make_pair("user_id", user_id), std::make_pair("duration", duration));
    }

    void set_group_anonymous_ban(const int64_t group_id, const Anonymous &anonymous, const int64_t duration) {
        print_api_call("set_group_anonymous_ban",
                       std::make_pair("group_id", group_id),
                       std::make_pair("anonymous", anonymous.name),
                       std::make_pair("duration", duration));
    }

    void set_group_whole_ban(const int64_t group_id, const bool enable) {
        print_api_call("set_group_whole_ban", std::make_pair("group_id", group_id), std::make_pair("enable", enable));
    }

    void set_group_admin(const int64_t group_id, const int64_t user_id, const bool enable) {
        print_api_call("set_group_admin", std::make_pair("group_id", group_id), std::make_pair("user_id", user_id), std::make_pair("enable", enable));
    }

    void set_group_anonymous(const int64_t group_id, const bool enable) {
        print_api_call("set_group_anonymous", std::make_pair("group_id", group_id), std::make_pair("enable", enable));
    }

    void set_group_card(const int64_t group_id, const int64_t user_id, const std::string &card) {
        print_api_call("set_group_card", std::make_pair("group_id", group_id), std::make_pair("user_id", user_id), std::make_pair("card", card));
    }

    void set_group_leave(const int64_t group_id, const bool dismiss) {
        print_api_call("set_group_leave", std::make_pair("group_id", group_id), std::make_pair("dismiss", dismiss));
    }

    void set_group_special_title(const int64_t group_id, const int64_t user_id, const std::string &special_title,
                                 const int64_t duration) {
        print_api_call("set_group_special_title",
                       std::make_pair("group_id", group_id),
                       std::make_pair("user_id", user_id),
                       std::make_pair("special_title", special_title),
                       std::make_pair("duration", duration));
    }

    void set_discuss_leave(const int64_t discuss_id) {
        print_api_call("set_discuss_leave", std::make_pair("discuss_id", discuss_id));
    }

#pragma endregion

#pragma region Request

    void set_friend_request(const RequestEvent::Flag &flag, const RequestEvent::Operation operation,
                            const std::string &remark) {
        print_api_call("set_friend_request", std::make_pair("flag", flag), std::make_pair("operation", operation), std::make_pair("remark", remark));
    }

    void set_group_request(const RequestEvent::Flag &flag, const GroupRequestEvent::SubType &sub_type,
                           const RequestEvent::Operation operation, const std::string &reason) {
        print_api_call("set_group_request",
                       std::make_pair("flag", flag),
                       std::make_pair("sub_type", sub_type),
                       std::make_pair("operation", operation),
                       std::make_pair("reason", reason));
    }

#pragma endregion

#pragma region QQ Information

    int64_t get_login_user_id() {
        print_api_call("get_login_user_id");
        return FAKE_LOGIN_USER_ID;
    }

    std::string get_login_nickname() {
        print_api_call("get_login_nickname");
        return FAKE_LOGIN_NICKNAME;
    }

    User get_stranger_info(const int64_t user_id, const bool no_cache) {
        print_api_call("get_stranger_info", std::make_pair("user_id", user_id), std::make_pair("no_cache", no_cache));
        User u;
        u.user_id = user_id;
        u.nickname = FAKE_OTHER_NICKNAME;
        u.sex = Sex::MALE;
        u.age = 20;
        return u;
    }

    std::vector<Friend> get_friend_list() {
        print_api_call("get_friend_list");
        Friend f;
        f.user_id = FAKE_OTHER_USER_ID;
        f.nickname = FAKE_OTHER_NICKNAME;
        f.remark = FAKE_NAME;
        return {f};
    }

    std::vector<Group> get_group_list() {
        print_api_call("get_group_list");
        return {};
    }

    Group get_group_info(const int64_t group_id, const bool no_cache) {
        print_api_call("get_group_info", std::make_pair("group_id", group_id), std::make_pair("no_cache", no_cache));
        return Group();
    }

    std::vector<GroupMember> get_group_member_list(const int64_t group_id) {
        print_api_call("get_group_member_list", std::make_pair("group_id", group_id));
        return {};
    }

    GroupMember get_group_member_info(const int64_t group_id, const int64_t user_id, const bool no_cache) {
        print_api_call(
            "get_group_info", std::make_pair("group_id", group_id), std::make_pair("user_id", user_id), std::make_pair("no_cache", no_cache));
        return GroupMember();
    }

#pragma endregion

#pragma region CoolQ

    std::string get_cookies(const std::string &domain) {
        print_api_call("get_cookies", std::make_pair("domain", domain));
        return "uin=" + to_string(FAKE_LOGIN_USER_ID);
    }

    int32_t get_csrf_token() {
        print_api_call("get_csrf_token");
        return 123;
    }

    std::string get_app_directory() {
        print_api_call("get_app_directory");
        const auto app_dir = fs::path(get_coolq_root_directory()) / "data" / "app" / "";
        dir::create_dir_if_not_exists(app_dir.string());
        return app_dir.string();
    }

    std::string get_coolq_root_directory() {
        print_api_call("get_coolq_root_directory");
        string root_dir = FAKE_COOLQ_ROOT_DIR;
        dir::create_dir_if_not_exists(root_dir);
        return fs::absolute(root_dir).string();
    }

    std::string get_image(const std::string &file) {
        print_api_call("get_image", std::make_pair("file", file));
        return "";
    }

    std::string get_record(const std::string &file, const std::string &out_format, const bool full_path) {
        print_api_call("get_record", std::make_pair("file", file), std::make_pair("out_format", out_format), std::make_pair("full_path", full_path));
        return "";
    }

    bool can_send_image() {
        print_api_call("can_send_image");
        return false;
    }

    bool can_send_record() {
        print_api_call("can_send_record");
        return false;
    }

    void add_log(const int32_t level, const std::string &tag, const std::string &message) {
        print_api_call("add_log", std::make_pair("level", level), std::make_pair("tag", tag), std::make_pair("message", message));
    }

#pragma endregion
} 
// namespace cq
