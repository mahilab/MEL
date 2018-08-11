namespace mel {

template <typename T>
Module<T>::Module() :
   values_(this),
   min_values_(this),
   max_values_(this)
{}

template <typename T>
Module<T>::~Module() { }

template <typename T>
bool Module<T>::set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values) {
    min_values_.set(min_values);
    max_values_.set(max_values);
    return true;
}

template <typename T>
bool Module<T>::set_range(uint32 channel_number, T min_value, T max_value) {
    if (validate_channel_number(channel_number)) {
        min_values_[channel_number] = min_value;
        max_values_[channel_number] = max_value;
        return true;
    }
    return false;
}

template <typename T>
std::vector<T>& Module<T>::get_values() {
    return values_.get();
}

template <typename T>
T Module<T>::get_value(uint32 channel_number) const {
    return values_[channel_number];
}

template <typename T>
void Module<T>::set_values(const std::vector<T>& values) {
    values_.set(values);
}

template <typename T>
void Module<T>::set_value(uint32 channel_number, T value) {
    values_[channel_number] = value;
}

} // namespace mel
