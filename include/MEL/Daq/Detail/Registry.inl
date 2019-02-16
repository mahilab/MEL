#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

namespace mel {

    template <typename T>
    Registry<T>::Registry(ModuleBase* module, T default_value) :
        RegistryBase(module),
        default_value_(default_value)
    { 
        values_.resize(this->module_->get_channel_count());        
        std::fill(values_.begin(), values_.end(), default_value_);
    }

    template <typename T>
    const T& Registry<T>::operator[](uint32 channel_number) const {
        return values_[index(channel_number)];
    }

    template <typename T>
    T& Registry<T>::operator[](uint32 channel_number) {
        return values_[index(channel_number)];
    }

    template <typename T>
    std::size_t Registry<T>::size() const {
        return values_.size();
    }

    template <typename T>
    std::vector<T>& Registry<T>::get() {
        return values_;
    }

    template <typename T>
    const std::vector<T> &Registry<T>::get() const {
        return values_;
    }

    template <typename T>
    void Registry<T>::set(const std::vector<T>& values) {
        if (values_.size() == values.size())
            values_ = values;
    }

    template <typename T>
    void Registry<T>::set_default_value(T default_value) {
        default_value_ = default_value;
    }

    template <typename T>
    void Registry<T>::change_channel_numbers(const std::map<uint32, std::size_t>& old_map,
                                const std::map<uint32, std::size_t>& new_map)
    {
        std::vector<T> new_values(new_map.size(), default_value_);
        for (auto it = old_map.begin(); it != old_map.end(); ++it) {
            if (new_map.count(it->first))
                new_values[new_map.at(it->first)] = values_[old_map.at(it->first)];
        }
        values_ = new_values;
    }

    /// Overload stream operator for Registry
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Registry<T>& registry) {
        if (registry.size() > 0) {
            os << "{";
            for (std::size_t i = 0; i < registry.size() - 1; i++) {
                uint32 ch = registry.module_->get_channel_numbers()[i];
                os << "[" << ch << "]:" << registry[ch] << ", ";
            }
            uint32 ch = registry.module_->get_channel_numbers()[registry.size() - 1];
            os << "[" << ch << "]:" << registry[ch] << "}";
        }
        else {
            os << "{}";
        }
        return os;
    }

} // namespace mel
