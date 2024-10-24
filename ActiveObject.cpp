#include <algorithm>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <utility>
#include <vector>

// class IsPrime
// {
// public:
//   std::pair<bool, int> operator()(int i)
//   {
//     for (int j = 2; j * j <= i; ++j)
//     {
//       if (i % j == 0)
//         return std::make_pair(false, i);
//     }
//     return std::make_pair(true, i);
//   }
// };

class ActiveObject
{
public:
  std::future<std::pair<bool, int>> enqueueTask(int i)
  {
    // IsPrime isPrime;
    std::packaged_task<std::pair<bool, int>(int)> newJob(isPrime);
    auto isPrimefuture = newJob.get_future();
    auto pair = std::make_pair(std::move(newJob), i);
    {
      std::lock_guard<std::mutex> lockGuard(activationListMutex);
      activationList.push_back(std::move(pair));
    }
    return isPrimefuture;
  }

  void run()
  {
    std::vector<std::thread> allServants;
    // only one thread because the activation list is the bottleneck
    for (int n = 0; n < 1; ++n)
    {
      allServants.emplace_back([this]
                               { while (!runNextTask()); });
    }
  }

private:
  bool runNextTask()
  {
    std::lock_guard<std::mutex> lockGuard(activationListMutex);
    auto empty = activationList.empty();
    if (!empty)
    {
      auto myTask = std::move(activationList.front());
      activationList.pop_front();
      myTask.first(myTask.second);
    }
    return empty;
  }

  std::deque<std::pair<std::packaged_task<std::pair<bool, int>(int)>, int>> activat ionList;
  std::mutex activationListMutex;
};

std::vector<int> getRandNumbers(int number)
{
  std::random_device seed;
  std::mt19937 engine(seed());
  std::uniform_int_distribution<> dist(1000000, 1000000000);
  std::vector<int> numbers;
  for (long long i = 0; i < number; ++i)
    numbers.push_back(dist(engine));
  return numbers;
}

std::future<std::vector<std::future<std::pair<bool, int>>>> getfutures(ActiveObject &activeObject, int numberPrimes)
{
  return std::async([&activeObject, numberPrimes]
                    {
    std::vector<std::future<std::pair<bool, int>>> futures;
    auto randNumbers = getRandNumbers(numberPrimes);
    for (auto numb : randNumbers) {
      futures.push_back(activeObject.enqueueTask(numb));
    }
    return futures; });
}

int main()
{
  std::cout << std::boolalpha << '\n';

  ActiveObject activeObject;

  // a few clients enqueue work concurrently
  auto client1 = getfutures(activeObject, 1998);
  auto client2 = getfutures(activeObject, 2003);
  auto client3 = getfutures(activeObject, 2011);
  auto client4 = getfutures(activeObject, 2014);
  auto client5 = getfutures(activeObject, 2017);

  // give me the std::futures
  auto futures = client1.get();
  auto futures2 = client2.get();
  auto futures3 = client3.get();
  auto futures4 = client4.get();
  auto futures5 = client5.get();

  // put all std::futures together
  futures.insert(futures.end(), std::make_move_iterator(futures2.begin()), std::make_move_iterator(futures2.end()));

  futures.insert(futures.end(), std::make_move_iterator(futures3.begin()), std::make_move_iterator(futures3.end()));

  futures.insert(futures.end(), std::make_move_iterator(futures4.begin()), std::make_move_iterator(futures4.end()));

  futures.insert(futures.end(), std::make_move_iterator(futures5.begin()), std::make_move_iterator(futures5.end()));

  // run the promises
  activeObject.run();

  // get the results from the std::futures
  std::vector<std::pair<bool, int>> futResults;
  futResults.reserve(futures.size());
  for (auto &fut : futures)
    futResults.push_back(fut.get());

  std::sort(futResults.begin(), futResults.end());

  // separate the primes from the non-primes
  auto prIt = std::find_if(futResults.begin(), futResults.end(),
                           [](std::pair<bool, int> pa)
                           { return pa.first == true; });

  std::cout << "Number primes: " << std::distance(prIt, futResults.end()) << '\n';
  std::cout << "Primes:" << '\n';
  std::for_each(prIt, futResults.end(), [](auto p)
                { std::cout << p.second << " "; });

  std::cout << "\n\n";

  std::cout << "Number no primes: " << std::distance(futResults.begin(), prIt) << '\n';
  std::cout << "No primes:" << '\n';
  std::for_each(futResults.begin(), prIt, [](auto p)
                { std::cout << p.second << " "; });

  std::cout << '\n';
}