#include "ArraySequence.h"
#include "FileReadOnlyStream.h"
#include "Generators.h"
#include "LazySequence.h"
#include "SubstringFrequencyCounter.h"

#include <iostream>
#include <limits>
#include <memory>
#include <string>

static int ReadInt(const std::string& prompt) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Некорректное число. Повторите ввод." << std::endl;
    }
}

static size_t ReadSize(const std::string& prompt) {
    size_t value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Некорректное число. Повторите ввод." << std::endl;
    }
}

static std::string ReadLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

static void PrintMenu() {
    std::cout << std::endl;
    std::cout << "1. Создать LazySequence<char> из строки" << std::endl;
    std::cout << "2. Создать бесконечную LazySequence<char> по циклическому шаблону" << std::endl;
    std::cout << "3. Прочитать строку из файла" << std::endl;
    std::cout << "4. Посчитать частоты подстрок длины k" << std::endl;
    std::cout << "5. Показать все частоты" << std::endl;
    std::cout << "6. Показать первые N элементов LazySequence" << std::endl;
    std::cout << "7. Показать количество материализованных элементов LazySequence" << std::endl;
    std::cout << "8. Показать пример Concat" << std::endl;
    std::cout << "9. Показать пример Interleave3 для трёх бесконечных последовательностей" << std::endl;
    std::cout << "10. Выйти" << std::endl;
}

static void PrintFrequencies(const ArraySequence<Pair<std::string, int> >& frequencies) {
    if (frequencies.IsEmpty()) {
        std::cout << "Частоты пока пустые." << std::endl;
        return;
    }
    for (int i = 0; i < frequencies.GetLength(); ++i) {
        std::cout << "\"" << frequencies.Get(i).first << "\" -> "
                  << frequencies.Get(i).second << std::endl;
    }
}

static void ShowFirstN(LazySequence<char>* sequence) {
    if (sequence == 0) {
        std::cout << "LazySequence не создана." << std::endl;
        return;
    }
    size_t count = ReadSize("N = ");
    std::cout << "Первые элементы: ";
    for (size_t i = 0; i < count; ++i) {
        try {
            std::cout << sequence->Get(static_cast<int>(i));
        } catch (const std::out_of_range&) {
            break;
        }
    }
    std::cout << std::endl;
}

static void ShowConcatExample() {
    char firstItems[2] = {'a', 'b'};
    char secondItems[2] = {'c', 'd'};
    LazySequence<char> first(firstItems, 2);
    LazySequence<char> second(secondItems, 2);
    std::unique_ptr<LazySequence<char> > finiteConcat(first.Concat(&second));
    std::cout << "Concat конечных последовательностей: ";
    for (int i = 0; i < 4; ++i) {
        std::cout << finiteConcat->Get(i);
    }
    std::cout << std::endl;

    LazySequence<char> infinite(new CyclicPatternCharGenerator("xy"));
    std::unique_ptr<LazySequence<char> > infiniteConcat(infinite.Concat(&second));
    std::cout << "Concat бесконечной и конечной, первые 10 элементов: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << infiniteConcat->Get(i);
    }
    std::cout << std::endl;
    std::cout << "Конечный хвост не достигается за конечное число чтений, потому что первая часть бесконечна."
              << std::endl;
}

static void ShowInterleave3Example() {
    LazySequence<int> first(new ArithmeticProgressionGenerator(0, 1));
    LazySequence<int> second(new ArithmeticProgressionGenerator(100, 1));
    LazySequence<int> third(new ArithmeticProgressionGenerator(200, 1));
    std::unique_ptr<LazySequence<int> > result(LazySequence<int>::Interleave3(&first, &second, &third));

    std::cout << "Первые 9 элементов Interleave3: ";
    for (int i = 0; i < 9; ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << result->Get(i);
    }
    std::cout << std::endl;
    std::cout << "Ожидается: 0, 100, 200, 1, 101, 201, 2, 102, 202" << std::endl;
}

static void ProcessCurrentSource(LazySequence<char>* currentLazy,
                                 ReadOnlyStream<char>* currentStream,
                                 SubstringFrequencyCounter& counter) {
    int k = ReadInt("k = ");
    if (currentLazy != 0) {
        Cardinal length = currentLazy->GetLength();
        size_t limit = 0;
        if (length.IsFinite()) {
            limit = length.Value();
        } else {
            limit = ReadSize("Последовательность бесконечная. Сколько символов обработать? ");
        }
        counter.ProcessLazySequence(currentLazy, k, limit);
    } else if (currentStream != 0) {
        currentStream->Open();
        counter.ProcessStream(currentStream, k);
        currentStream->Close();
    } else {
        std::cout << "Сначала создайте LazySequence или поток." << std::endl;
        return;
    }

    std::cout << "Обработано символов: " << counter.GetTotalProcessedCharacters() << std::endl;
    std::cout << "Найдено подстрок: " << counter.GetTotalSubstrings() << std::endl;
    std::cout << "Уникальных подстрок: " << counter.GetUniqueSubstringCount() << std::endl;
}

int main() {
    std::unique_ptr<LazySequence<char> > currentLazy;
    std::unique_ptr<ReadOnlyStream<char> > currentStream;
    SubstringFrequencyCounter counter;
    CharDeserializer charDeserializer;

    std::cout << "Лабораторная работа 4: ленивые последовательности, потоки, частоты подстрок" << std::endl;

    bool running = true;
    while (running) {
        PrintMenu();
        int command = ReadInt("Выберите пункт: ");

        try {
            if (command == 1) {
                std::string data = ReadLine("Введите строку: ");
                currentLazy.reset(new LazySequence<char>(new StringCharGenerator(data)));
                currentStream.reset();
                std::cout << "LazySequence<char> создана из строки длины " << data.size() << "." << std::endl;
            } else if (command == 2) {
                std::string pattern = ReadLine("Введите непустой шаблон: ");
                currentLazy.reset(new LazySequence<char>(new CyclicPatternCharGenerator(pattern)));
                currentStream.reset();
                std::cout << "Бесконечная LazySequence<char> создана." << std::endl;
            } else if (command == 3) {
                std::string path = ReadLine("Путь к файлу: ");
                currentStream.reset(new FileReadOnlyStream<char>(path, &charDeserializer));
                currentLazy.reset();
                std::cout << "Файловый поток создан. Файл будет читаться посимвольно." << std::endl;
            } else if (command == 4) {
                ProcessCurrentSource(currentLazy.get(), currentStream.get(), counter);
            } else if (command == 5) {
                PrintFrequencies(counter.GetAllFrequencies());
            } else if (command == 6) {
                ShowFirstN(currentLazy.get());
            } else if (command == 7) {
                if (currentLazy.get() == 0) {
                    std::cout << "LazySequence не создана." << std::endl;
                } else {
                    std::cout << "Материализовано элементов: "
                              << currentLazy->GetMaterializedCount() << std::endl;
                }
            } else if (command == 8) {
                ShowConcatExample();
            } else if (command == 9) {
                ShowInterleave3Example();
            } else if (command == 10) {
                running = false;
            } else {
                std::cout << "Неизвестная команда." << std::endl;
            }
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << std::endl;
        }
    }

    return 0;
}
