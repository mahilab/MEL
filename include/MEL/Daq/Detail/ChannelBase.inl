namespace mel {

template <typename T>
ChannelBase<T>::ChannelBase() : module_(nullptr), channel_number_(0) {}

template <typename T>
ChannelBase<T>::ChannelBase(Module<T>* module, uint32 channel_number)
    : module_(module), channel_number_(channel_number) {}

template <typename T>
ChannelBase<T>::~ChannelBase() {}

template <typename T>
bool ChannelBase<T>::update() {
    return module_->update_channel(channel_number_);
}

template <typename T>
T ChannelBase<T>::get_value() const {
    return module_->get_value(channel_number_);
}

template <typename T>
void ChannelBase<T>::set_value(T value) {
    module_->set_value(channel_number_, value);
}

template <typename T>
void ChannelBase<T>::operator= (const T& value) {
    set_value(value);
}

template <typename T>
ChannelBase<T>::operator T() const {
    return get_value();
}

template <typename T>
uint32 ChannelBase<T>::get_channel_number() const {
    return channel_number_;
}

template <typename T>
bool ChannelBase<T>::is_valid() const {
    if (module_ == nullptr)
        return false;
    if (!module_->validate_channel_number(channel_number_))
        return false;
    return true;
}

} // namespace mel
