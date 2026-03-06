// colours based on ConsoleColor.h

// Copyleft Vincent Godin
// Expanded by Kran

#pragma once

inline std::ostream& blue(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& red(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& green(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& yellow(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& white(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return s;
}

inline std::ostream& magenta(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& gray(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& cyan(std::ostream& s)
{
    const HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_stdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return s;
}

struct color
{
    explicit color(const WORD attribute): m_color(attribute)
    {
    }

    WORD m_color;
};

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& i, color& c)
{
    const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout, c.m_color);
    return i;
}

typedef std::ostream& (*ColorFunc)(std::ostream&);

class console_queue
{
    std::queue<std::pair<std::string, ColorFunc>> messages_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
    std::thread worker_;

public:
    console_queue()
    {
        worker_ = std::thread([this]
        {
            while (true)
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [this] { return !messages_.empty() || stop_; });

                if (stop_ && messages_.empty())
                    return;

                messages_.front().second(std::cout) << messages_.front().first << white;
                messages_.pop();
            }
        });
    }

    ~console_queue()
    {
        {
            std::lock_guard lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        worker_.join();
    }

    // Push a message to the console queue with a specified color
    void push(const std::string& message, ColorFunc color)
    {
        {
            std::lock_guard lock(mtx_);
            messages_.emplace(message, color);
        }
        cv_.notify_all();
    }
};
