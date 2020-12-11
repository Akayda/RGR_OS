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
	mutex mutex; //мьютекс, будет заблокирован, когда философ поднимет вилку, и разблокирован, когда он положит ее.
};

struct table
{
	array<fork, num_philosophers> forks; //массив вилок
};

class philosopher
{
private:
	string const name; //имя
	table const& dinnertable; //указатель на стол
	fork& left_fork; //указатель на вилку слева
	fork& right_fork;//указатель на вилку справа
	thread lifethread;
	mt19937 rng{ random_device{}() }; //случайное число
public:
	philosopher(string n, table const& t, fork& l, fork& r) :
		name(n), dinnertable(t), left_fork(l), right_fork(r), lifethread(&philosopher::dine, this)
	{
	}

	~philosopher()
	{
		lifethread.join(); //приостанавливает выполнение
	}

	void dine()
	{
		do
		{
			think(); //думать
			eat(); //кушать
		} while (true);
	}

	void print(string text)
	{
		lock_guard<mutex> cout_lock(g_lockprint);
		cout << name << text << endl; 
	}

	void eat()
	{
		lock(left_fork.mutex, right_fork.mutex); //запрос мьютексу

		lock_guard<mutex> left_lock(left_fork.mutex, adopt_lock);
		lock_guard<mutex> right_lock(right_fork.mutex, adopt_lock);

		print(" начал трапезу.");

		static thread_local uniform_int_distribution<> dist(1, 6); //генерация случайного числа
		this_thread::sleep_for(chrono::milliseconds(dist(rng) * 50)); //ожидание

		print(" закончил трапезу.");
	}

	void think()
	{
		static thread_local uniform_int_distribution<> wait(1, 6);//генерация случ чисел
		this_thread::sleep_for(chrono::milliseconds(wait(rng) * 150));//ожидание

		print(" размышляет.");
	}
};

void dine()
{
	this_thread::sleep_for(chrono::seconds(1));
	cout << "Начало обеда" << endl;
	table table;
	array<philosopher, num_philosophers> philosophers
		{
		{
		{ "Сократ", table, table.forks[0], table.forks[1] },
		{ "Эпикур", table, table.forks[1], table.forks[2] },
		{ "Платон", table, table.forks[2], table.forks[3] },
		{ "Шиллер", table, table.forks[3], table.forks[4] },
		{ "Декарт", table, table.forks[4], table.forks[0] },
		}
		};
}

int main()
{
	setlocale(LC_ALL, "Russian");
	dine();

	return 0;
}