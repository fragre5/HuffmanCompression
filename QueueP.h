#pragma once
#include <iostream>
#include <new>

template <typename T>
class QueueP
{
private:
	T* A; // динамический массив элементов типа T
	int* P{}; // массив приоритетов
	int count; // кол-во элементов

public:
	//конструктор без параметров
	QueueP() { count = 0; }

	//конструктор копировани€
	QueueP(const QueueP& _Q)
	{
		try
		{
			A = new T[_Q.count]; //выделить пам€ть под массив элементов
			P = new int[_Q.count]; //выделить пам€ть под массив приоритетов
		}
		catch (std::bad_alloc exception)
		{
			//если пам€ть не выделилась, то обрабатываем ошибку
			std::cout << exception.what() << std::endl;
			count = 0;
			return;
		}

		//скопировать _Q => *this
		count = _Q.count;

		//поочердное копирование данных
		for (int i = 0; i < count; i++)
			A[i] = _Q.A[i];

		for (int i = 0; i < count; i++)
			P[i] = _Q.P[i];
	}

	//деструктор
	~QueueP()
	{
		if (count > 0)
		{
			delete[] A;
			delete[] P;
		}
	}

	//оператор копировани€
	QueueP operator=(const QueueP& _Q);

	//добавление элемента
	void push(T item, int priority);

	//выт€гивание первого элемента из очереди
	T pop();

	//очистка очереди
	void clear()
	{
		if (count > 0)
		{
			delete[] A;
			delete[] P;
			count = 0;
		}
	}

	//возвращение количества элементов в очереди
	int Count()
	{
		return count;
	}

	//функци€ вывод€ща€ очередь
	void Print(const char* objName);

	void printTest(const char* ojbName);
};

//оператор копировани€
template <typename T>
QueueP<T> QueueP<T>::operator=(const QueueP& _Q)
{
	if (count > 0)
	{
		delete[] A;
		delete[] P;
		count = 0;
	}

	try
	{
		A = new T[_Q.count];
		P = new int[_Q.count];
	}
	catch (std::bad_alloc exception)
	{
		std::cout << exception.what() << std::endl;
		return *this;
	}

	count = _Q.count;

	for (int i = 0; i < count; i++)
	{
		A[i] = _Q.A[i];
		P[i] = _Q.P[i];
	}
	return *this;
}

//добавление в очередь новый элемент с приоритетом 
template <typename T>
void QueueP<T>::push(T item, int priority)
{
	T* A2;
	int* P2;

	try
	{
		A2 = (T*)new T[count + 1];
		P2 = (int*)new int[count + 1];
	}
	catch (std::bad_alloc exception)
	{
		std::cout << exception.what() << std::endl;
		return;
	}

	//ѕоиск позиции (pos) в массиве (P) согласно с приоритетом (priority)
	int pos;

	if (count == 0)
		pos = 0;
	else
	{
		pos = 0;
		while (pos < count)
		{
			if (P[pos] > priority) break;
			pos++;
		}
	}

	for (int i = 0; i < pos; i++)
	{
		A2[i] = A[i];
		P2[i] = P[i];
	}

	//добавить элемент в позиции pos
	A2[pos] = item;
	P2[pos] = priority;

	//элементы после позиции pos
	for (int i = pos + 1; i < count + 1; i++)
	{
		A2[i] = A[i - 1];
		P2[i] = P[i - 1];
	}

	//освобождение пам€ти выделенную под A и P
	if (count > 0)
	{
		delete[] A;
		delete[] P;
	}

	//A->A2, P->P2
	A = A2;
	P = P2;
	count++;
}

//выт€гивание элемент из очереди
template <typename T>
T QueueP<T>::pop()
{
	if (count == 0)
		return 0;

	T* A2;
	int* P2;

	try
	{
		A2 = new T[count - 1]; //на 1 элемент меньше
		P2 = new int[count - 1];
	}
	catch (std::bad_alloc exception)
	{
		std::cout << exception.what() << std::endl;
		return 0;
	}

	T item;
	item = A[0]; //запоминаем первый элемент

	for (int i = 0; i < count - 1; i++)
	{
		A2[i] = A[i + 1];
		P2[i] = P[i + 1];
	}

	if (count > 0)
	{
		delete[] A;
		delete[] P;
	}

	count--;

	A = A2;
	P = P2;

	return item;
}

//вывод очереди на экран
template <typename T>
void QueueP<T>::Print(const char* objName)
{
	for (int i = 0; i < count; i++) {
		std::cout << *(A[i]) << " ";
	}
		
		
	std::cout << std::endl;
}

//вывод очереди на экран дл€ тестов
template <typename T>
void QueueP<T>::printTest(const char* objName)
{
	std::cout << objName << '\n';
	for (int i = 0; i < count; i++)
	{
		std::cout << A[i] << " : " << P[i] << std::endl;
	}
	std::cout << std::endl;
}
