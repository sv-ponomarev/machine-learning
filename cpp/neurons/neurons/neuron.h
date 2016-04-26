/*! \file  neuron.h
����� ����� ������� + ������� ���������� ���������, ������������ � ��������.
��� ������������ ���������� ����������� ������� ���������, ������������� ������� � ����������� ������������� �������
(��� �������������� ������� ���������).
*/
#pragma once
#include <Eigen/Dense>
#include <set>
#include <ctime>
#include <cstdlib>

using Eigen::MatrixXd;
using Eigen::VectorXd;

class Neuron {

public:
	/*
	Neuron
	w - ������ ����� ������� ������������ (n + 1, 1), ��� n - ����� ���������, w[0, 0] - ��������
	activation_function - ������������� ������� �������
	activation_function_derivative - ����������� ������������� ������� ������� (��� ������������ ������)
	cost_function - ������� ���������
	*/
	Neuron(VectorXd w) {
		this->w = w;
	}
	
	// ��� ������������ �� ������������ ������ ������� ���������� ����������� ��������� ��� ������:
	virtual VectorXd activation_function(VectorXd x) = 0;
	virtual VectorXd activation_function_derivative(VectorXd x) = 0;
	virtual double cost_function(Neuron &n, MatrixXd X, VectorXd y) = 0;

	// ������ ������ ����������� 2 ������� ���������: ������������ � ���������������
	double J_quadratic(Neuron &n, MatrixXd X, VectorXd y);
	double J_logarifmic(Neuron &n, MatrixXd X, VectorXd y);

	/*
	forward_pass
	������������� ������� ������� ��� ���������� �������
	single_input - ������ ������������ (n + 1, 1), ��� n - ����� ���������
	*/
	double forward_pass(VectorXd single_input) {
		VectorXd sum = Eigen::VectorXd::Constant(1, 0.0);
		for (int i = 0; i != this->w.rows(); i++) {
			sum(0) += this->w(i) * single_input(i);
		}
		return activation_function(sum)(0,0);
	}
	/*
	summatory
	����������� ������� �������. ��������� ������������ ����� � �������� ��������� ��� ������ ��������.
	input_matrix - ������������ (m, n + 1), ��� m - ����� ��������, n - ����� ���������.
	���������� ������ ���� ��� ������ �������� ������������ (m, 1).
	*/
	VectorXd summatory(MatrixXd input_matrix) {
		return input_matrix * this->w;
	}
	/*
	activation
	������������� ������� ������� ��� ������ ��������. �������� �� ���� ��������� ������ ����������� �������,
	������ summatory_activation ������������ (m, 1), ��� m - ����� ��������.
	���������� ������ (m, 1) �� ���������� ��������� ��� ���� ��������.
	*/
	VectorXd activation(VectorXd summatory_activation) {
		return activation_function(summatory_activation);
	}
	/*
	vectorized_forward_pass
	��������������� ������������� ������� ��� ������ ��������.
	input_matrix - ������� ������������ (m, n + 1), ��� m - ����� ��������, n - ����� ���������.
	���������� ������ (m, 1) �� ���������� ��������� ��� ���� ��������.
	*/
	VectorXd vectorized_forward_pass(MatrixXd input_matrix) {
		return activation(summatory(input_matrix));
	}
	/*
	GD
	������� ���� ��������� ������������ ������.
	X - ������� ������� ��������� (m, n + 1)
	y - ������ ���������� ������� (m, 1)
	learning_rate - ��������� �������� ��������
	batch_size - ������ ��������� �������
	eps - �������� ��������� - �������� �������� ����� ���������� ������� ������� ��� �������� ��������
	max_steps - �������� ��������� �2 - ����������� ��������� ����� ��������
	����� ���������� 1, ���� �������� ������ �������� ��������� (�������� �������), � 0,
	���� �������� ������ �������� (�������� �� ����� �������)
	*/
	int GD(MatrixXd X, VectorXd y, std::vector<double> &costs, int batch_size = 2, double learning_rate = 1.0, double eps = 1e-15, int max_steps = 200) {
		for (int step = 0; step < max_steps; step++) {
			std::set<int> numbers;
			srand((unsigned int)time(NULL));
			int m = X.rows();
			int n = X.cols();
			while (numbers.size() != batch_size) {
				int idx = rand() % m;
				numbers.insert(idx);
			}
			std::set<int>::iterator it; 
			MatrixXd batchX = Eigen::MatrixXd::Constant(batch_size, n, 1.0);
			VectorXd batchy = Eigen::VectorXd::Constant(batch_size, 0.0);
			int i = 0;
			for (it = numbers.begin(); it != numbers.end(); ++it) {
				//batchX.block(i, 0, 1, m) = X.block(*it, 0, 1, m);
				for (int j = 0; j < n; j++)
					batchX(i, j) = X(*it, j);
				batchy(i) = y(*it);
				i++;
			}
			double cost = cost_function(*this, X, y);
			costs.push_back(cost);
			if (update_mini_batch(batchX, batchy, learning_rate, eps))
				return 1;
		}
		return 0;
	}
	/*
	update_mini_batch
	���������� ����� GD - ������������ �������� � ��������� ���� �������.
	X - ������� ������� (batch_size, n + 1)
	y - ������ ���������� ������� ������� (batch_size, 1)
	learning_rate - ��������� �������� ��������
	eps - �������� ��������� ����� ����: ���� ������� ����� ��������� ������� ������� 
	�� � ����� ���������� ����� ������ eps - �������� ���������������.
	���������� 1 ��� ������������ ��������, 0 � ��������� �������
	*/
	int update_mini_batch(MatrixXd X, VectorXd y, double learning_rate, double eps) {
		double J = cost_function(*this, X, y);
		VectorXd grad = compute_grad_analitically(*this, X, y);
		this->w -= learning_rate * grad;
		return (abs(J - cost_function(*this, X, y)) < eps);
	}

	/*
	��������� ������ ������� ����������� ������� ��������� ��� ������� ����������
	*/
	VectorXd J_derivative(VectorXd y, VectorXd y_hat) {
		return (y_hat - y)/y.rows();
	}

	/*
	compute_grad_analitically
	������������� ����������� ������� ���������.
    neuron - ������ ������ Neuron
    X - ������������ ������� ������ ����� (m, n + 1), �� ������� ��������� ����� ��������� ����������
    y - ���������� ������ ��� �������� �� ������� X
    J_prime - �������, ��������� ����������� ������� ������� �� �������
    ���������� ������ ������� (m, 1)
	*/
	VectorXd compute_grad_analitically(Neuron &n, MatrixXd X, VectorXd y) {
		VectorXd z = n.summatory(X); // z - ������ ����������� ����������� ������� ������� �� ������ ��������
		VectorXd y_hat = n.activation(z); // �������� ������������� ������� ��� ���� ��������
		VectorXd dy_dyhat = J_derivative(y, y_hat); // ����������� ������� ���������
		VectorXd dyhat_dz = n.activation_function_derivative(z); // ����������� ������������� �������
		MatrixXd dz_dw = X; // ����������� �� ��������� ������������� ������� (x*w)' = x
		VectorXd mult = dy_dyhat *  dyhat_dz; 
		VectorXd grad = mult.transpose()*dz_dw; // ���������� ������� �����������
		return grad.transpose(); // ������� �� ������ � �������
	}
	/*
	compute_grad_numerically_2
	��������� ����������� ������� ���������
    neuron - ������ ������ Neuron � ������������ �������� ����� w,
    X - ������������ ������� ������ ����� (n, m), �� ������� ��������� ����� ��������� ����������,
    y - ���������� ������ ��� �������� ������� X,
    J - ������� �������, �������� ������� �� ����� ��������,
    eps - ������ delta w (������ ��������� �����).
    ���������� ������ ������� (m, 1)
	*/
	VectorXd compute_grad_numerically_2(Neuron &n, MatrixXd X, VectorXd y, double eps=1e-9) {
		VectorXd w_0 = n.w;
		VectorXd num_grad2 = Eigen::VectorXd::Constant(n.w.rows(), 0.0);
		for (int i = 0; i != n.w.rows(); i++) {
			double old_w = w_0(i);
			n.w(i) -= eps;
			double Jminus = n.cost_function(n, X, y);
			n.w(i) = old_w + eps;
			double Jplus = n.cost_function(n, X, y);
			num_grad2(i) = (Jplus - Jminus)/(2*eps);
			n.w(i) = old_w;
		}
		return num_grad2;
	}
	VectorXd w;
};