namespace mel {

    template<typename T>
    Input<T>::Input() : 
        Module<T>()
    {}

    template<typename T>
    Input<T>::Input(const std::vector<uint32>& channel_numbers) : 
        Module<T>(channel_numbers) 
    { }

    template<typename T>
    Input<T>::~Input() {}

    template<typename T>
    typename Input<T>::Channel Input<T>::get_channel(uint32 channel_number) {
        if (this->validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    template<typename T>
    std::vector<typename Input<T>::Channel> Input<T>::get_channels(
        const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    template<typename T>
    typename Input<T>::Channel Input<T>::operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    template<typename T>
    std::vector<typename Input<T>::Channel> Input<T>::operator[](const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

    template <typename T>
    Input<T>::Channel::Channel() : ChannelBase<T>() { }

    template <typename T>
    Input<T>::Channel::Channel(Input* module, uint32 channel_number)
        : ChannelBase<T>(module, channel_number) { }

} // namespace mel
