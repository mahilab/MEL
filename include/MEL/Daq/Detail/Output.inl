namespace mel {

    template <typename T>
    Output<T>::Output() :
        Module<T>(),
        enable_values_(this),
        disable_values_(this),
        expire_values_(this)
    {
    }

    template <typename T>
    Output<T>::Output(const std::vector<uint32>& channel_numbers) :
        Module<T>(channel_numbers),
        enable_values_(this),
        disable_values_(this),
        expire_values_(this)
    {
    }

    template <typename T>
    Output<T>::~Output() {}

    template <typename T>
    bool Output<T>::set_expire_values(const std::vector<T>& expire_values) {
        expire_values_.set(expire_values);
        return true;
    }

    template <typename T>
    bool Output<T>::set_expire_value(uint32 channel_number, T expire_value) {
        if (Module<T>::validate_channel_number(channel_number)) {
            expire_values_[channel_number] = expire_value;
            return true;
        }
        return false;
    }

    template <typename T>
    void Output<T>::set_enable_values(const std::vector<T>& enable_values) {
        if (this->validate_channel_count(enable_values.size()))
            enable_values_.set(enable_values);
    }

    template <typename T>
    void Output<T>::set_enable_value(uint32 channel_number, T enable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            enable_values_[channel_number] = enable_value;
    }

    template <typename T>
    void Output<T>::set_disable_values(const std::vector<T>& disable_values) {
        if (this->validate_channel_count(disable_values.size()))
            disable_values_.set(disable_values);
    }

    template <typename T>
    void Output<T>::set_disable_value(uint32 channel_number, T disable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            disable_values_[channel_number] = disable_value;
    }

    template <typename T>
    typename Output<T>::Channel Output<T>::get_channel(uint32 channel_number) {
        if (Module<T>::validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    template <typename T>
    std::vector<typename Output<T>::Channel> Output<T>::get_channels(
        const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    template <typename T>
    typename Output<T>::Channel Output<T>::operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    template <typename T>
    std::vector<typename Output<T>::Channel> Output<T>::operator[](
        const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

    template <typename T>
    bool Output<T>::on_enable() {
        this->set_values(enable_values_.get());
        return this->update();
    }

    template <typename T>
    bool Output<T>::on_disable() {
        this->set_values(disable_values_.get());
        return this->update();
    }

    template <typename T>
    Output<T>::Channel::Channel() : ChannelBase<T>() {}

    template <typename T>
    Output<T>::Channel::Channel(Output* module, uint32 channel_number)
        : ChannelBase<T>(module, channel_number) {}

    template <typename T>
    void Output<T>::Channel::set_enable_value(T enable_value) {
        dynamic_cast<Output<T>*>(this->module_)->set_enable_value(this->channel_number_, enable_value);
    }

    template <typename T>
    void Output<T>::Channel::set_disable_value(T disable_value) {
        dynamic_cast<Output<T>*>(this->module_)->set_disable_value(this->channel_number_, disable_value);
    }

    template <typename T>
    bool Output<T>::Channel::set_expire_value(T expire_value) {
        return dynamic_cast<Output<T>*>(this->module_)->set_expire_value(this->channel_number_, expire_value);
    }

} // namespace mel
