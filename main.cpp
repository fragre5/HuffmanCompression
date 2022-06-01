#include <iostream>
#include "QueueP.h"
#include <iostream>
#include <fstream>
#include "Node.h"
#include <bitset>
#include <filesystem>
#include <iomanip>

void read_file(const std::string& filename, int* frequency) 
{
    std::ifstream ifs(filename, std::ifstream::binary);

    while (!ifs.eof())
    {
        char ch;
        ifs.read(&ch, 1);
        if (ifs.eof())
        {
            break;
        }
        frequency[static_cast<unsigned char>(ch)]++;
    }
    std::cout << '\n';
    std::cout << '\n';
}

void make_code(Node::pointer& node, std::string str, std::string* codes) 
{
    if (node->left != nullptr)
    {
        make_code(node->left, str + "0", codes);
    }
    if (node->right != nullptr)
    {
        make_code(node->right, str + "1", codes);
    }

    if (node->left == nullptr && node->right == nullptr)
    {
        node->code(str);
        codes[node->get_byte()] = str;
        //std::cout << "лист: " << *node << " с кодом: " << node->code() << '\n';
    }
}

void fill_queue(int* frequency, QueueP<Node::pointer>& queue)
{
    for (int i = 0; i < 0x100; i++)
    {
        if (frequency[i] != 0) {
            Node::pointer node = std::make_shared<Node>(i, frequency[i]);
            queue.push(node, frequency[i]);
        }
    }
}

void build_tree(QueueP<Node::pointer>& queue)
{
    while (queue.Count() > 1)
    {
        Node::pointer x = queue.pop();
        Node::pointer y = queue.pop();

        std::string name = x->get_name() + y->get_name(); //получение имени нового узла
        Node::pointer z = std::make_shared<Node>(name, x->get_frequency() + y->get_frequency());

        z->left = x;
        z->right = y;

        x->parent = z;
        y->parent = z;
        queue.push(z, x->get_frequency() + y->get_frequency());
    }
}

std::string message2code(const std::string& filename, std::string* codes)
{
    std::string msg{ "" };
    std::ifstream ifs(filename, std::ifstream::binary);
    
    while (true)
    {
        char ch;
        ifs.read(&ch, 1);

        if (ifs.eof())
        {
            break;
        }

        msg += codes[static_cast<uchar>(ch)];
    }
    return msg;
}

void write_file(const std::string& filename, int* frequency, QueueP<Node::pointer>& queue, const std::string message)
{
    std::string new_filename = filename + ".hff";
    std::ofstream ofs(new_filename, std::ofstream::binary);

    uchar count = 0;
    for (int i = 0; i < 0x100; i++)
    {
        if (frequency[i] != 0)
        {
            count++;
        }
    }
    ofs.write(reinterpret_cast<char*>(&count), sizeof count);

    uchar count1 = 0;
    for (int i = 0; i < 0x100; i++)
    {
        if (frequency[i] != 0)
        {
           ofs.write(reinterpret_cast<char*>(&count1), sizeof count1);
           //Записываем его частоту
           ofs.write(reinterpret_cast<char*>(&frequency[i]), sizeof frequency[i]);
        }
        count1++;
    }

    int byte_count = message.size() / 8; //
    uchar modulo = message.size() % 8;
    
    ofs.write(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
    ofs.write(reinterpret_cast<char*>(&modulo), sizeof modulo);

    //Запись всех битов в файл
    int i = 0;
    for (; i < byte_count; ++i)
    {
        std::bitset<8> b(message.substr(i * 8, 8));
        uchar value = static_cast<uchar>(b.to_ulong());
        ofs.write(reinterpret_cast<char*>(&value), sizeof value);
    }
    if (modulo > 0)
    {
        std::bitset<8> b(message.substr(i * 8, modulo));
        uchar value = static_cast<uchar>(b.to_ulong());
        ofs.write(reinterpret_cast<char*>(&value), sizeof value);
    }
}

void read_decoding_file(std::string& filename, int* frequency, std::string& message)
{
    std::string new_filename = filename + ".hff";
    std::ifstream ifs(new_filename, std::ifstream::binary);

    uchar count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof count);

    int i = 0;
    while (i < count)
    {
        uchar ch;
        ifs.read(reinterpret_cast<char*>(&ch), sizeof ch);

        int f = 0;
        ifs.read(reinterpret_cast<char*>(&f), sizeof f);
        frequency[ch] = f;
        ++i;
    }

    int byte_count = 0;
    uchar modulo = 0;
    ifs.read(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
    ifs.read(reinterpret_cast<char*>(&modulo), sizeof modulo);

    i = 0;
    for (; i < byte_count; ++i)
    {
        uchar byte;
        ifs.read(reinterpret_cast<char*>(&byte), sizeof byte);

        std::bitset<8> b(byte);
        message += b.to_string();
    }

    if (modulo > 0)
    {
        uchar byte;
        ifs.read(reinterpret_cast<char*>(&byte), sizeof byte);

        std::bitset<8> b(byte);
        message += b.to_string().substr(8-modulo, 8);
    }
}

void make_char(const Node::pointer root, const std::string& message, std::string& text)
{
    Node::pointer node = root;

    for (size_t i = 0; i < message.size(); ++i)
    {
        char ch = message[i];
        if (ch == '0')  //если текущий символ 0 и у узла есть левый ребонок 
        {
            if (node->left != nullptr)
            {
                node = node->left;
                if (node->is_leaf())
                {
                    text += node->get_byte();
                    node = root;
                }
            }
        }
        else if (ch == '1')
        {
            if (node->right != nullptr)
            {
                node = node->right;
                if (node->is_leaf())
                {
                    text += node->get_byte();
                    node = root;
                }
            }
        }
    }
    /*std::cout << "Расшифрованный текст:\n";
    std::cout << "-------------------\n";
    std::cout << text << "\n";
    std::cout << "-------------------\n";*/
}

void write_decoding_file(const std::string& filename, const std::string& text)
{
    std::string new_filename = filename + ".decode";
    std::ofstream ofs(new_filename, std::ostream::binary);

    ofs << text;
}

void weight_of_codefile(const std::string& filename, std::string& bits)
{
    std::string code_file = filename + ".hff";
    std::ifstream ifs(code_file, std::ifstream::binary);
    while (!ifs.eof())
    {
        std::string input;
        ifs >> input;
        bits += input;
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

   

    std::string file;
    std::cout << "Введите имя файла, который будем шифровать!\n";
    std::cin >> file;
    std::string filename = "C:\\Work\\" + file + ".txt";

    if (!std::ifstream(filename))
    {
        std::cerr << "Указанного вами файла - не существует!\n";
        return -1;
    }


    std::ifstream ifs(filename);
    if (ifs.peek() == EOF)
    {
        std::cerr << "Внимание! Файл пуст!\n";
        std::cerr << "Пустой файл нельзя закодировать!\n";
        return -1;
    }


    int* frequency = new int[0x100]{ 0 };
    read_file(filename, frequency);
    int count = 0;
    long long countOfSymbols = 0;
    for (int i = 0; i < 0x100; i++)
    {
        if (frequency[i] != 0) 
        {
            countOfSymbols += frequency[i];
            count++;
        }
    }

    if (count == 2 && frequency[10] != 0 && frequency[13] != 0)
    {
        std::cerr << "Внимание! Файл пуст!\n";
        return -1;
    }

    if ((count == 3 && frequency[10] != 0 && frequency[13] != 0 && frequency[32] != 0) || (count == 1 && frequency[32] != 0))
    {
        std::cerr << "Внимание! Файл пуст!\n";
        return -1;
    }


    QueueP<Node::pointer> queue;

    fill_queue(frequency, queue);
    build_tree(queue);

    std::string* codes = new std::string[0x100]{ "" };
    Node::pointer root = queue.pop();
    make_code(root, "", codes);

    std::string message = message2code(filename, codes);

    write_file(filename, frequency, queue, message);

    int* frequency2 = new int[0x100]{ 0 };
    std::string message2 = "";

    read_decoding_file(filename, frequency2, message2);

    QueueP<Node::pointer> queue2;
    fill_queue(frequency2, queue2);
    build_tree(queue2);
    Node::pointer root2 = queue2.pop();
    std::string text = "";
    make_char(root2, message2, text);

    write_decoding_file(filename, text);

    std::cout << "Файл " << file + ".txt" << " найден и успешно зашифрован!\n\n";

    std::cout << "Зашифрованное сообщение записано в файл: " << file + ".txt.hff\n\n";
    std::cout << "Расшифрованное сообщение записано в файл: " << file + ".txt.decode\n\n";

    double original_file = text.size() / 1000.0;
    
    std::cout << "Размер исходного файла равен: " << original_file << " КилоБайт\n";
    
    std::string bits = "";
    weight_of_codefile(filename, bits);
    double code_file = bits.size() / 1000.0;

    std::cout << "Размер сжатого файла равен: " << code_file << " КилоБайт\n";

    double compression = original_file / code_file;

    if (compression > 1)
    {
        std::cout << "Исходный файл сжался в: " << std::setprecision(2) << original_file / code_file << " раза\n";
    }
    else
    {
        std::cout << "Внимание! Видимо исходный файл был слижком мал, поэтому сжатие не помогло уменьшить размер файла!\n";
        std::cout << "Исходный файл сжался в: " << std::setprecision(2) << original_file / code_file << " раза\n";
        std::cout << "Запомните, чем больше файл, тем сжатие эффективнее!\n";   
    }

    return 0;
}
