#pragma once

class Encoder {

public:

	int counts_per_revolution_;
	int quadrature_factor_;

	void get_encoder_counts();

};

