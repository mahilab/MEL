#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

namespace mel {

    template <typename T>
    Buffer<T>::Buffer(ModuleBase* module, T default_value) :
        BufferBase(module),
        default_value_(default_value)
    { }

    template <typename T>
    const T& Buffer<T>::operator[](uint32 channel_number) const {
        return values_[index(channel_number)];
    }

    template <typename T>
    T& Buffer<T>::operator[](uint32 channel_number) {
        return values_[index(channel_number)];
    }

    template <typename T>
    std::size_t Buffer<T>::size() const {
        return values_.size();
    }

    template <typename T>
    std::vector<T>& Buffer<T>::get() {
        return values_;
    }

    template <typename T>
    void Buffer<T>::set(const std::vector<T>& values) {
        if (values_.size() == values.size())
            values_ = values;
    }

    template <typename T>
    void Buffer<T>::set_default_value(T default_value) {
        default_value_ = default_value;
    }

    template <typename T>
    void Buffer<T>::change_channel_numbers(const std::map<uint32, std::size_t>& old_map,
                                const std::map<uint32, std::size_t>& new_map)
    {
        std::vector<T> new_values(new_map.size(), default_value_);
        for (auto it = old_map.begin(); it != old_map.end(); ++it) {
            if (new_map.count(it->first))
                new_values[new_map.at(it->first)] = values_[old_map.at(it->first)];
        }
        values_ = new_values;
    }

    /// Overload stream operator for Buffer
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Buffer<T>& container) {
        if (container.size() > 0) {
            os << "{";
            for (std::size_t i = 0; i < container.size() - 1; i++) {
                uint32 ch = container.module_->get_channel_numbers()[i];
                os << "[" << ch << "]:" << container[ch] << ", ";
            }
            uint32 ch = container.module_->get_channel_numbers()[container.size() - 1];
            os << "[" << ch << "]:" << container[ch] << "}";
        }
        else {
            os << "{}";
        }
        return os;
    }

} // namespace mel
