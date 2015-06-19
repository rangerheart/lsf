// File:        completion_queue.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <functional>
#include <map>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// Function Closure
////////////////////////////////////////////////////////////
struct AsyncInfo
{
public:
    AsyncInfo() : fd(0), accept_fd(0) { buffer.clear(); }
    void Clear() { fd = 0; accept_fd = 0; buffer.clear(); }

public:
    int         fd;
    int         accept_fd;
    std::string buffer;
};

struct CompletionFunc
{
public:
    static const int ACTION_ACCEPT     = 1;
    static const int ACTION_READ       = 2;
    static const int ACTION_PEER_CLOSE = 3;
    static const int ACTION_CONNECT    = 4;
    static const int ACTION_WRITE      = 5;
    static const int ACTION_TIMER      = 6;

    typedef std::function<bool(AsyncInfo &)>  func_type;

public:
    int         action;
    func_type   func;
    std::string buffer;
};

////////////////////////////////////////////////////////////
// Completion Queue
////////////////////////////////////////////////////////////
class CompletionQueue : 
    public basic::NonCopyable,
    public basic::Error
{
public:
    typedef std::map<int,CompletionFunc> func_map_type;

    const static size_t DEL_QUEUE_SIZE = 65536;
    
public:
    template<typename HandlerType>
    bool AddCompletionTask(int fd, int action, HandlerType func, void const * buffer = NULL, size_t buflen = 0)
    {
        CompletionFunc * pfunc = NULL;
        switch (action)
        {
            case CompletionFunc::ACTION_ACCEPT:
            case CompletionFunc::ACTION_READ:
            case CompletionFunc::ACTION_TIMER:
                pfunc = &_read_func[fd];
                break;
                
            case CompletionFunc::ACTION_WRITE:
            case CompletionFunc::ACTION_CONNECT:
                pfunc = &_write_func[fd];
                break;

            case CompletionFunc::ACTION_PEER_CLOSE:
                pfunc = &_peer_close_func[fd];
                break;

            default:
                break;
        }
        if (pfunc == NULL) return false;

        pfunc->action = action;
        pfunc->func = CompletionFunc::func_type(func);
        pfunc->buffer.assign((char *)buffer, buflen);
        return true;
    }

    void CancelCompletionTask(int fd)
    {
        _read_func.erase(fd);
        _write_func.erase(fd);
        _peer_close_func.erase(fd);
    }

    bool GetReadCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _read_func.find(fd);
        if (iter == _read_func.end()) return false;
        *pfunc = &iter->second;
        return true;
    }

    bool GetPeerCloseCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _peer_close_func.find(fd);
        if (iter == _peer_close_func.end()) return false;
        *pfunc = &iter->second;
        return true;
    }

    bool GetWriteCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _write_func.find(fd);
        if (iter == _write_func.end()) return false;
        *pfunc = &iter->second;
        return true;

    }

private:
    func_map_type   _read_func;
    func_map_type   _write_func;
    func_map_type   _peer_close_func;;
};

} // end of namespace async
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
