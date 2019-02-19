namespace mel {

    template <typename T>
    InputOutput<T>::InputOutput() :
        Output<T>(),
        directions_(this, Direction::In)
    {
        sort_input_output_channel_numbers();
    }

    template <typename T>
    InputOutput<T>::InputOutput(const ChanNums& channel_numbers) :
        Output<T>(channel_numbers),
        directions_(this, Direction::In)
    {
        sort_input_output_channel_numbers();
    }

    template <typename T>
    InputOutput<T>::~InputOutput() {}

    template <typename T>
    bool InputOutput<T>::set_directions(const std::vector<Direction>& directions) {
        directions_.set(directions);
        sort_input_output_channel_numbers();
        return true;
    }

    template <typename T>
    bool InputOutput<T>::set_direction(ChanNum channel_number, Direction direction) {
        if (this->validate_channel_number(channel_number)) {
            directions_[channel_number] = direction;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

    template <typename T>
    bool InputOutput<T>::update_input() {
        auto chnums = get_input_channel_numbers();
        bool success = true;
        for (auto& c : chnums)
            success = update_channel(c) ? success : false;
        return success;
    }

    template <typename T>
    bool InputOutput<T>::update_output() {
        auto chnums = get_output_channel_numbers();
        bool success = true;
        for (auto& c : chnums)
            success = update_channel(c) ? success : false;
        return success;
    }

    template <typename T>
    std::size_t InputOutput<T>::get_input_channel_count() const {
        return input_channel_numbers_.size();
    }

    template <typename T>
    std::size_t InputOutput<T>::get_output_channel_count() const {
        return output_channel_numbers_.size();
    }

    template <typename T>
    const ChanNums& InputOutput<T>::get_input_channel_numbers() const {
        return input_channel_numbers_;
    }

    template <typename T>
    const ChanNums& InputOutput<T>::get_output_channel_numbers() const {
        return output_channel_numbers_;
    }

    template <typename T>
    typename InputOutput<T>::Channel InputOutput<T>::get_channel(ChanNum channel_number) {
        if (this->validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    template <typename T>
    std::vector<typename InputOutput<T>::Channel> InputOutput<T>::get_channels(
        const ChanNums& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    template <typename T>
    typename InputOutput<T>::Channel InputOutput<T>::operator[](ChanNum channel_number) {
        return get_channel(channel_number);
    }

    template <typename T>
    std::vector<typename InputOutput<T>::Channel> InputOutput<T>::operator[](
        const ChanNums& channel_numbers) {
        return get_channels(channel_numbers);
    }

    template <typename T>
    void InputOutput<T>::sort_input_output_channel_numbers() const {
        input_channel_numbers_.clear();
        output_channel_numbers_.clear();
        for (std::size_t i = 0; i < this->get_channel_count(); ++i) {
            if (directions_.get()[i] == In)
                input_channel_numbers_.push_back(this->get_channel_numbers()[i]);
            else if (directions_.get()[i] == Out)
                output_channel_numbers_.push_back(this->get_channel_numbers()[i]);
        }
    }

    template <typename T>
    InputOutput<T>::Channel::Channel() : ChannelBase<T>() {}

    template <typename T>
    InputOutput<T>::Channel::Channel(InputOutput* module, ChanNum channel_number)
        : ChannelBase<T>(module, channel_number) {}

    template <typename T>
    void InputOutput<T>::Channel::set_direction(Direction direction) {
        dynamic_cast<InputOutput<T>*>(this->module_)->set_direction(this->channel_number_, direction);
    }

} // namespace mel
