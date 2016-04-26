/*! \file  perceptron.h
����� ����������� �� ���������� �������� ��� ������������ �������� �������� (������ ���������� ������ �� ����� ��������).
*/
#pragma once
#include <Eigen/Dense>
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

class Perceptron {

public:
	/*
	Perceptron
	������������� �����������
	w - ������ ����� ������������ (n + 1, 1), n - ����� ���������
	*/
	Perceptron(VectorXd w) {
		this->w = w;
	}
	/*
	forward_pass
	������ ������ ����������� ��� ������������ ������ �������
    single_input - ������ ������� ������������ (n + 1, 1)
    ����� ���������� True/False 
	*/
	int forward_pass(VectorXd single_input) {
		double result = 0;
		for (int i = 0; i != this->w.rows(); i++) {
			result += (this->w(i) * single_input(i));
		}
		return (result > 0);
	}
	/*
	vectorized_forward_pass
	������ ������� ������� ����������� ��� ������������ ������ ��������
	input_matrix - ������� ������������ (m, n + 1), ��� m - ����� ����������,
	n - ����� ���������
	����� ���������� ������ ������� ����������� (True/False) ������������ (m, 1)
	*/
	MatrixXd vectorized_forward_pass(MatrixXd input_matrix) {
		MatrixXd mult = input_matrix * this->w;
		MatrixXd result = Eigen::MatrixXd::Constant(mult.rows(), 1,  0.0);
		for (int i = 0; i != mult.rows(); i++) {
			if (mult(i) > 0)
				result(i) = 1;
		}
		return result;
	}
	/*
	train_on_single_example
	��������� ������ ��������� ������ example (n + 1, 1)
	� ��������� ����� y (0 / 1). ��������� �������� ����� � ������ ������.
	���������� ������ ������ (0 ��� 1).
	*/
	int train_on_single_example(MatrixXd example, int y) {
		MatrixXd response = vectorized_forward_pass(example);
		int r = (int)response(0, 0);
		if (r != y) {
			this->w += (y - r) * example;
			return 1;
		}
		else
			return 0;
	}

	double count_errors(MatrixXd input_matrix, VectorXd y) {
		VectorXd prediction = vectorized_forward_pass(input_matrix);
		return (prediction - y).cwiseAbs().sum();
	}
	/*
	train_until_convergence
	�������� ����������� �� ������ ��������.
	input_matrix - ������� �������� (m, n + 1), ���
	m - ����� ��������, n - ����� ���������
	y - ������ � ����������� �������� (True/False) ������������ (m, 1)
	max_steps - ������������ ���������� �������� ���������
	*/
	int train_until_convergence(MatrixXd input_matrix, VectorXd y, std::vector<double> &err, int max_steps = 10000) {
		int i = 0;
		int errors = 1;
		err.push_back(count_errors(input_matrix, y));
		while ((errors != 0) && (i < max_steps)) {
			i++;
			errors = 0;
			for (int j = 0; j != input_matrix.rows(); j++) {
				MatrixXd example = input_matrix.block(j, 0, 1, input_matrix.cols());
				int answer = (int)y(j);
				int error = train_on_single_example(example, answer);
				errors += error;
			}
			err.push_back(count_errors(input_matrix, y));
		}
		if (errors == 0)
			return 1;
		else
			return 0;
	}

	VectorXd w;
};

/*
create_perceptron
������� ���������� �� ���������� ������ � ��������� ���������.
*/
Perceptron create_perceptron(int n) {
	VectorXd w = VectorXd::Random(n + 1);
	w(0) = 1;
	return Perceptron(w);
}