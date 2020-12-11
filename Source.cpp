#include <array>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <random>
using namespace std;
mutex g_lockprint;
#define num_philosophers 5

struct fork
{
	mutex mutex; //�������, ����� ������������, ����� ������� �������� �����, � �������������, ����� �� ������� ��.
};

struct table
{
	array<fork, num_philosophers> forks; //������ �����
};

class philosopher
{
private:
	string const name; //���
	table const& dinnertable; //��������� �� ����
	fork& left_fork; //��������� �� ����� �����
	fork& right_fork;//��������� �� ����� ������
	thread lifethread;
	mt19937 rng{ random_device{}() }; //��������� �����
public:
	philosopher(string n, table const& t, fork& l, fork& r) :
		name(n), dinnertable(t), left_fork(l), right_fork(r), lifethread(&philosopher::dine, this)
	{
	}

	~philosopher()
	{
		lifethread.join(); //���������������� ����������
	}

	void dine()
	{
		do
		{
			think(); //������
			eat(); //������
		} while (true);
	}

	void print(string text)
	{
		lock_guard<mutex> cout_lock(g_lockprint);
		cout << name << text << endl; 
	}

	void eat()
	{
		lock(left_fork.mutex, right_fork.mutex); //������ ��������

		lock_guard<mutex> left_lock(left_fork.mutex, adopt_lock);
		lock_guard<mutex> right_lock(right_fork.mutex, adopt_lock);

		print(" ����� �������.");

		static thread_local uniform_int_distribution<> dist(1, 6); //��������� ���������� �����
		this_thread::sleep_for(chrono::milliseconds(dist(rng) * 50)); //��������

		print(" �������� �������.");
	}

	void think()
	{
		static thread_local uniform_int_distribution<> wait(1, 6);//��������� ���� �����
		this_thread::sleep_for(chrono::milliseconds(wait(rng) * 150));//��������

		print(" ����������.");
	}
};

void dine()
{
	this_thread::sleep_for(chrono::seconds(1));
	cout << "������ �����" << endl;
	table table;
	array<philosopher, num_philosophers> philosophers
		{
		{
		{ "������", table, table.forks[0], table.forks[1] },
		{ "������", table, table.forks[1], table.forks[2] },
		{ "������", table, table.forks[2], table.forks[3] },
		{ "������", table, table.forks[3], table.forks[4] },
		{ "������", table, table.forks[4], table.forks[0] },
		}
		};
}

int main()
{
	setlocale(LC_ALL, "Russian");
	dine();

	return 0;
}