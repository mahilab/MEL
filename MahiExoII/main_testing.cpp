#include <iostream>
#include <csignal>
#include "MahiExoIIEmgFrc.h"
#include <boost/circular_buffer.hpp>



int main(int argc, char * argv[]) {

    
    /*mel::uint32 id_emg = 0;
    mel::uint32 id_ati = 1;
    if (!check_digital_loopback(0, 7)) {
        mel::print("Warning: Digital loopback not connected to Q8Usb 0");
        id_emg = 1;
        id_ati = 0;
    }

    //  create a Q8Usb object
    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };

    mel::Q8Usb::Options options;
    for (int i = 0; i < 8; ++i) {
        options.do_initial_signals_[i] = 1;
        options.do_final_signals_[i] = 1;
        options.do_expire_signals_[i] = 1;
    }

    mel::Daq* q8_emg = new mel::Q8Usb(id_emg, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    //  create a second Q8Usb object
    ai_channels = { 0, 1, 2, 3, 4, 5 };
    ao_channels = {};
    di_channels = {};
    do_channels = {};
    enc_channels = {};

    mel::Daq* q8_ati = new mel::Q8Usb(id_ati, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

    MahiExoIIEmgFrc::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_emg->do_(i);
        config.command_[i] = q8_emg->ao_(i);
        config.encoder_[i] = q8_emg->encoder_(i);
        config.encrate_[i] = q8_emg->encrate_(i);
    }
    for (int i = 0; i < 8; ++i) {
        config.emg_[i] = q8_emg->ai_(i);
    }
    for (int i = 0; i < 6; ++i) {
        config.wrist_force_sensor_[i] = q8_ati->ai_(i);
    }
    MahiExoIIEmgFrc* meii = new MahiExoIIEmgFrc(config);*/

    mel::double_vec trode_0 = { 0.2760,    0.4598, - 0.2175,    0.7961, - 1.5180, - 1.0749, - 3.0722,    0.5214, - 0.9910, - 0.2531,    1.0093,    0.0510, - 0.4404, - 0.8485, - 0.2404,    0.6029, - 1.5163, - 0.0683,    0.7824, - 1.4207 };
    mel::double_vec trode_1 = { 0.6694,    0.6832, - 0.8821, - 1.5045,    0.4331,    0.8083,    0.5789,    0.7628, - 1.1821,    0.5838, - 0.5800, - 0.5614, - 1.5559,    1.1002,    0.1751,    1.0036,    1.5110, - 1.1372,    0.6430, - 0.0128 };
    mel::double_vec trode_2 = { 0.9143,    1.1077,    0.8205, - 0.8176, - 0.1265,    0.2641,    3.1585,    1.2266,    2.3206,    0.4145,    0.2118,    0.6132, - 0.5278,    1.2416, - 0.1576, - 1.3736,    0.8708, - 1.5685, - 1.8443,    0.2884 };
    mel::double_vec trode_3 = { -0.9509, - 0.9107, - 0.1619, - 0.4889, - 0.2228,    0.2721, - 1.1685, - 1.2242, - 2.0995, - 0.3902,    0.6643, - 0.7023,    0.5013,    0.5403,    0.9908,    0.9894, - 0.6888, - 0.8568,    0.0484, - 0.6649 };

    int num_channels_= 4;
    mel::double_vec emg_voltages = mel::double_vec(num_channels_, 0);
    struct EmgDataBuffer {
        EmgDataBuffer(size_t num_channels, size_t length) :
            num_channels_(num_channels),
            length_(length)
        {
            for (int i = 0; i < num_channels_; ++i) {
                data_buffer_.push_back(boost::circular_buffer<double>(length_));
            }
        }
        void push_back(mel::double_vec data_vec) {
            if (data_vec.size() != num_channels_) {
                mel::print("ERROR: Incorrect number of rows when calling EmgDataBuffer::push_back().");
            }
            for (int i = 0; i < num_channels_; ++i) {
                data_buffer_[i].push_back(data_vec[i]);
            }
        }
        mel::double_vec at(int index) {
            mel::double_vec data_vec;
            for (int i = 0; i < num_channels_; ++i) {
                data_vec.push_back(data_buffer_[i][index]);
            }
            return data_vec;
        }
        mel::double_vec get_channel(int index) {
            mel::double_vec channel_vec;
            for (int i = 0; i < length_; ++i) {
                channel_vec.push_back(data_buffer_[index][i]);
            }
            return channel_vec;
        }
        size_t num_channels_;
        size_t length_;
        std::vector<boost::circular_buffer<double>> data_buffer_;
    };
    EmgDataBuffer emg_data_buffer = EmgDataBuffer(num_channels_, 20);

    for (int i = 0; i < 20; ++i) {
        emg_voltages[0] = trode_0[i];
        emg_voltages[1] = trode_1[i];
        emg_voltages[2] = trode_2[i];
        emg_voltages[3] = trode_3[i];
        emg_data_buffer.push_back(emg_voltages);
    }

    struct FeatureExtractTest {

        size_t num_channels_ = 4;
        size_t num_features_ = 9;

        mel::double_vec feature_extract(EmgDataBuffer& emg_data_buffer) {

            mel::double_vec feature_vec;
            feature_vec.reserve(num_channels_*num_features_);
            mel::double_vec nrms_vec;
            mel::double_vec nmav_vec;
            mel::double_vec nwl_vec;
            mel::double_vec nzc_vec;
            mel::double_vec nssc_vec;
            mel::double_vec ar_vec(4);
            mel::double_vec ar1_vec;
            mel::double_vec ar2_vec;
            mel::double_vec ar3_vec;
            mel::double_vec ar4_vec;

            // extract unnormalized features
            for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
                nrms_vec.push_back(rms_feature_extract(emg_data_buffer.data_buffer_[i]));
                nmav_vec.push_back(mav_feature_extract(emg_data_buffer.data_buffer_[i]));
                nwl_vec.push_back(wl_feature_extract(emg_data_buffer.data_buffer_[i]));
                nzc_vec.push_back(zc_feature_extract(emg_data_buffer.data_buffer_[i]));
                nssc_vec.push_back(ssc_feature_extract(emg_data_buffer.data_buffer_[i]));
                ar4_feature_extract(ar_vec,emg_data_buffer.get_channel(i));
                
                ar1_vec.push_back(ar_vec[0]);
                ar2_vec.push_back(ar_vec[1]);
                ar3_vec.push_back(ar_vec[2]);
                ar4_vec.push_back(ar_vec[3]);
            }

            // normalize features
            double rms_mean = 0;
            double mav_mean = 0;
            double wl_mean = 0;
            double zc_mean = 0;
            double ssc_mean = 0;
            for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
                rms_mean += nrms_vec[i] / num_channels_;
                mav_mean += nmav_vec[i] / num_channels_;
                wl_mean += nwl_vec[i] / num_channels_;
                zc_mean += nzc_vec[i] / num_channels_;
                ssc_mean += nssc_vec[i] / num_channels_;
            }
            for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
                nrms_vec[i] = nrms_vec[i] / rms_mean;
                nmav_vec[i] = nmav_vec[i] / mav_mean;
                nwl_vec[i] = nwl_vec[i] / wl_mean;
                if (zc_mean > 0) {
                    nzc_vec[i] = nzc_vec[i] / zc_mean;
                }
                if (ssc_mean > 0) {
                    nssc_vec[i] = nssc_vec[i] / ssc_mean;
                }
            }

            // copy features into one vector (inserted in reverse order)
            auto it = feature_vec.begin();
            it = feature_vec.insert(it, ar4_vec.begin(), ar4_vec.end());
            it = feature_vec.insert(it, ar3_vec.begin(), ar3_vec.end());
            it = feature_vec.insert(it, ar2_vec.begin(), ar2_vec.end());
            it = feature_vec.insert(it, ar1_vec.begin(), ar1_vec.end());
            it = feature_vec.insert(it, nssc_vec.begin(), nssc_vec.end());
            it = feature_vec.insert(it, nzc_vec.begin(), nzc_vec.end());
            it = feature_vec.insert(it, nwl_vec.begin(), nwl_vec.end());
            it = feature_vec.insert(it, nmav_vec.begin(), nmav_vec.end());
            it = feature_vec.insert(it, nrms_vec.begin(), nrms_vec.end());
            return feature_vec;
        }

        double rms_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
            double sum_squares = 0;
            for (int i = 0; i < emg_channel_buffer.size(); ++i) {
                sum_squares += std::pow(emg_channel_buffer[i], 2);
            }
            return std::sqrt(sum_squares / emg_channel_buffer.size());
        }

        double mav_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
            double sum_abs = 0;
            for (int i = 0; i < emg_channel_buffer.size(); ++i) {
                sum_abs += std::abs(emg_channel_buffer[i]);
            }
            return sum_abs / emg_channel_buffer.size();
        }

        double wl_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
            double sum_abs_diff = 0;
            for (int i = 0; i < emg_channel_buffer.size() - 1; ++i) {
                sum_abs_diff += std::abs(emg_channel_buffer[i + 1] - emg_channel_buffer[i]);
            }
            return sum_abs_diff;
        }

        double zc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
            double sum_abs_diff_sign = 0;
            for (int i = 0; i < emg_channel_buffer.size() - 1; ++i) {
                sum_abs_diff_sign += std::abs(std::copysign(1.0, emg_channel_buffer[i + 1]) - std::copysign(1.0, emg_channel_buffer[i]));
            }
            return sum_abs_diff_sign / 2;
        }

        double ssc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
            double sum_abs_diff_sign_diff = 0;
            for (int i = 0; i < emg_channel_buffer.size() - 2; ++i) {
                sum_abs_diff_sign_diff += std::abs(std::copysign(1.0, (emg_channel_buffer[i + 2] - emg_channel_buffer[i + 1])) - std::copysign(1.0, (emg_channel_buffer[i + 1] - emg_channel_buffer[i])));
            }
            return sum_abs_diff_sign_diff / 2;
        }

        void ar4_feature_extract(mel::double_vec& coeffs,const mel::double_vec& emg_channel_buffer) {

            // initialize
            size_t N = emg_channel_buffer.size();
            size_t m = coeffs.size();
            mel::print(m);
            mel::double_vec A_k(m + 1, 0.0);
            A_k[0] = 1.0;
            mel::double_vec f(emg_channel_buffer);
            mel::double_vec b(emg_channel_buffer);
            double D_k = 0;
            for (size_t j = 0; j <= N; ++j) {
                D_k += 2.0 * std::pow(f[j], 2);
            }
            D_k -= std::pow(f[0], 2) + std::pow(b[N], 2);
            
            // Burg recursion
            for (size_t k = 0; k < m; ++k) {

                // compute mu
                double mu = 0.0;
                for (size_t n = 0; n <= N - k - 1; ++n) {
                    mu += f[n + k + 1] * b[n];
                }
                mu *= -2.0 / D_k;

                // update A_k
                for (size_t n = 0; n <= (k + 1) / 2; ++n) {
                    double t1 = A_k[n] + mu * A_k[k + 1 - n];
                    double t2 = A_k[k + 1 - n] + mu * A_k[n];
                    A_k[n] = t1;
                    A_k[k + 1 - n] = t2;
                }

                // update f and b
                for (size_t n = 0; n <= N - k - 1; ++n) {
                    double t1 = f[n + k + 1] + mu * b[n];
                    double t2 = b[n] + mu * f[n + k + 1];
                    f[n + k + 1] = t1;
                    b[n] = t2;
                }

                // update D_k
                D_k = (1.0 - std::pow(mu, 2)) * D_k - std::pow(f[k + 1], 2) - std::pow(b[N - k - 1], 2);
            }
            
            // assign coefficients
            coeffs.assign(++A_k.begin(), A_k.end());
        }
    };

    FeatureExtractTest fet;
    mel::double_vec feature_vec(36);
    feature_vec = fet.feature_extract(emg_data_buffer);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << feature_vec[i*4 + j] << " ";
        }
        std::cout << std::endl;
    }
    

    return 0;

}