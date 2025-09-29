#pragma once

namespace dts {


class Any {
public:
    Any() = default;    

    Any(const Any& other) 
        : content_(other.content_ ? other.content_->GetCopy() : nullptr)
    {}

    Any& operator=(const Any& other) {
        if (this == &other) {
            return *this;
        }
        content_ = nullptr;
        content_ = other.content_ ? other.content_->GetCopy() : nullptr;
        return *this;
    }

    Any(Any&& other) noexcept 
        : content_(other.content_)    
    {
        other.content_ = nullptr;
    }

    Any& operator=(Any&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        content_ = nullptr;
        content_ = other.content_;
        other.content_ = nullptr;
        return *this;
    }

    ~Any() { 
        delete content_; 
    }

public:
    template<typename T>
    Any(const T& value) 
        : content_(new Holder<T>(value))
    {}

    template<typename T>
    Any& operator=(const T& value) {
        content_ = new Holder<T>(value);
        return *this;
    }

public:
    void Reset() {
        delete content_;
    }

    template<typename T>
    bool Contains() const {
        return dynamic_cast<Holder<T>*>(content_) != nullptr;
    }

    void Swap(Any& other) noexcept {
        std::swap(content_, other.content_);
    }

    template<typename T, typename... Args>
    void Emplace(Args&&... args) {
        Reset();
        content_ = new Holder<T>(T(std::forward<Args>(args)...));
    }

private:
    struct PlHolder {
        virtual PlHolder* GetCopy() const = 0;
        virtual ~PlHolder() = default;
    };

    template<typename T>
    struct Holder : public PlHolder {
    public:
        Holder(const Holder& other) = delete;

        Holder& operator=(const Holder& other) = delete;

        Holder(Holder&& other) = delete;

        Holder& operator=(Holder&& other) = delete;

        ~Holder() = default;
        
        Holder(const T& par) :
            value(par)
        {}

        Holder(T&& par) :
            value(std::move(par))
        {}

        Holder* GetCopy() const override {
            return (new Holder(value));
        }

    public:
        T value;
    };

private:
    template<typename T>
    friend T& AnyCast(Any& other);

private:
    PlHolder* content_;
};


template<typename T>
T& AnyCast(Any& other) {
    auto* ptr = dynamic_cast<Any::Holder<T>*>(other.content_);
    if (!ptr) {
        throw std::bad_cast();
    }
    return ptr->value;
}


}