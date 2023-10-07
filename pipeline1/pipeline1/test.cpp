#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <queue>
#include <iostream>

#include "qylog.h"

class Data {
public:
	Data(int val) : value(val) {
	}

public:
	int value;
};

class Stage {
public:
	virtual void process(Data *pData) = 0;

	Stage(std::string value) : name(value) {
	}

	void run() {
		while (true) {
			{
				std::unique_lock<std::mutex> lock(mtx);
				cv.wait(lock, [this]() {
					return dataQueue.size();
				});
			}

			while (true) {
				Data *pData = nullptr;
				{
					std::unique_lock<std::mutex> lock(mtx);
					if (dataQueue.empty())
						break;

					pData = dataQueue.front();
					dataQueue.pop();
				}

				std::cout << name << " --> data = " << pData->value << std::endl;
				process(pData);
				std::cout << name << " <-- data = " << pData->value << std::endl;

				notifyStage(pData);
			}
		}
	}

public:
	void addData(Data *pData) {
		std::lock_guard<std::mutex> lock(mtx);

		// enable below code to process latest data only
		std::queue<Data *> tmpQueue;
		std::swap(dataQueue, tmpQueue);

		dataQueue.push(pData);

		std::cout << "--> " << name << " data = " << pData->value << std::endl;
		cv.notify_all();
	}

	void addStage(Stage *pStage) {
		nextStages.push_back(pStage);
	}

	void notifyStage(Data *pData) {
		for (Stage *pStage : nextStages) {
			pStage->addData(pData);
		}
	}

public:
	std::vector<Stage *> nextStages;

	std::string name;
	std::queue<Data *> dataQueue;
	std::mutex mtx;
	std::condition_variable cv;
};

class Stage1 : public Stage {
public:
	Stage1() : Stage("Stage1") {}

	void process(Data *pData) override {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
};

class Stage2 : public Stage {
public:
	Stage2() : Stage("Stage2") {}

	void process(Data *pData) override {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
};

class Stage3 : public Stage {
public:
	Stage3() : Stage("Stage3") {}

	void process(Data *pData) override {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
};

void main(int argc, const char *argv[]) {
	Stage *pStage1 = new Stage1();
	Stage *pStage2 = new Stage2();
	Stage *pStage3 = new Stage3();
	
	pStage1->addStage(pStage2);
	pStage2->addStage(pStage3);

	std::vector<std::thread> threads;
	threads.emplace_back([pStage1]() {
		pStage1->run();
	});

	threads.emplace_back([pStage2]() {
		pStage2->run();
	});

	threads.emplace_back([pStage3]() {
		pStage3->run();
	});

	for (int i = 0; i < 20; i++) {
		Data *pData = new Data(i + 1);
		pStage1->addData(pData);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	for (std::thread& thread : threads) {
		thread.join();
	}

	system("pause");
}
