#include <iostream>
#include <vector>
#include <cmath>
#include <clocale>
#include <sstream>

#define PARITYMAX 4

/// Disciplina: 12325 - COMUNICAÇÃO DIGITAL
/// Atividade: Avaliação 1 – Aplicação Hamming (14,10), (12,8) e CRC de 10 bits.
/// Programa 01
/// Acadêmico: Sydney Matheus de Souza


/// CRC a ser utilizado de 10 bits
/// Hamming elaborado de 12,8 e 14,10
/// Código de Aluno 6987028 -> 028

using namespace std;

void doubleBitInsert (vector<bool> &word)
{
    int sum = 0;
    for(int i = 0; i<word.size(); i++)
        sum = sum + word[i];

    sum = sum%2;                                        //Faz o XOR de todos os Bits
    word.insert(word.begin(), sum);                     //Acrescenta esse bit na frente de todos

    cout<<endl<<"A mensagem a ser enviada com o bit de detecção de 2 erros será: "<<endl;
    for(int i = 0; i<word.size(); i ++)
        cout<<word[i]<<" ";
    cout<<endl;
}

int doubleBitVerify (vector<bool> &word)
{
    cout<<endl<<"A mensagem recebida com o bit ''P'' de detecção de 2 erros: "<<endl;
    for(int i = 0; i<word.size(); i ++)
        cout<<word[i]<<" ";
    cout<<endl;

    bool erroP = 0; int sum = 0;
    for(int i = 1; i<word.size(); i++)
        sum = sum + word[i];

    sum = sum%2;                                        // Faz o XOR de todos os bits menos o primeiro

    if(sum == word[0])                                  // Se o número de bits do XOR for igual ao bit P, está certo retorna 0 (sem erro no bit P)
    {
        word.erase(word.begin());                       // Remove esse bit P da palavra de dados
        return erroP = 0;
    }
    else
    {
        word.erase(word.begin());                       // Se o XOR atual der diferente do bit P, a palavra de dados está com erro
        return erroP = 1;
    }
}
                                                        // #### Tabela de erros do bit P###
                                                        //  C      P          Descrição
                                                        //  0      0           Sem erro
                                                        //  0      1          Erro no P
                                                        //  >0     0            2 Erros
                                                        //  >0     1            1 Erro
                                                        //
                                                        //  C = Resultado de k0 a k7
                                                        //  P = Bit P de verificação de 2 erros
                                                        //  Acima de 2 erros saída imprevisível

void hamming14_10(vector<vector<bool>> matriz, vector<bool> &word, vector<int> &parityBit)
{
    for(int i = 0; i < word.size(); i++)                // Neste for é definido p1 a p8 pelo objeto parityBit. No vector de paridade é feito uma soma sucessiva fazendo um AND
        for(int j = PARITYMAX-1; j>=0; j--)             // do bit da palavra com uma tabela de binários que indica se aquela posição faz parte de cada bit de paridade
            parityBit[-1*(j-PARITYMAX+1)] = parityBit[-1*(j-PARITYMAX+1)]+(matriz[i+1][j]*word[i]);
                                                        //Por exemplo da matriz temos que se estiver no i = 2 (terceira posição) o p1 deve seguir a lógica de ter o bit mais a
                                                        //direita igual a 1 para proteger, logo compreende a posição i = 2 pois, Matriz [bit] [[0][1][2][3]]<- Pos. do bit. Ex Matriz[3][3] = 1 ja que [0][0][1][1]
    for(int i = 0; i<PARITYMAX; i++)
        parityBit[i]= parityBit[i]%2;                   // Após ter a soma de cada cojunto de bit que pertence a cada bit de paridade é finalizado o XOR, fazendo
                                                        // MOD com o 2, que indica se a soma era par ou impár.
}

void hamming14_10Verify(vector<vector<bool>> matriz, vector<bool> &word, vector<int> &parityBit, vector<bool> wordError)
{
    int erro = 0, erroP = 0;
    erroP = doubleBitVerify(wordError);                 // Verifica se o P recebi está igual ou diferente igual retorna 0 diferente retorna 1

    hamming14_10( matriz, wordError, parityBit);        // Executasse a função de hamming na palavra com erro, ja que essa parte se mantem e o hamming anterior ja incluia
                                                        // os bits de proteção, so que antes zerados agora possuem valor.
    cout<<endl<<"A mensagem original enviada sem o bit ''P'' foi: "<<endl;
    for(int i = 1; i<word.size(); i ++)
        cout<<word[i]<<" ";
    cout<<endl;
    cout<<endl<<"A mensagem recebida sem o bit ''P'' foi: "<<endl;
    for(int i = 0; i<wordError.size(); i ++)
        cout<<wordError[i]<<" ";
    cout<<endl;
    cout<<endl<<"Os bits de verificação k7 a k0, foram respectivamente: "<<endl;
    for(int i = PARITYMAX; i>0; i--)
    {
        cout<<parityBit[i-1]<<" ";
        erro = erro + parityBit[i-1];
    }
    cout<<endl<<endl;

    if(erro>0)                                          // Se encontrar algum erro print na tela os parêmtros qual bit do erro, msg com erro corrigido e
    {                                                   // chama a atenção que essa correção não está necessariamente esta correta
        if(erroP == 1)
        {
            erro = 0;
            cout<<endl<<endl<<"Erro detectado no bit: "<<endl;
            for(int i = 0; i<PARITYMAX; i++)            // Conversão binário -> Decimal
                erro = erro + parityBit[i]*pow(2,(i));

            cout<<erro<<endl;
            wordError[erro-1] = !(wordError[erro-1]);
            cout<<endl<<"Mensagem com erro corrigido*: "<<endl;
            for(int i = 0; i<wordError.size(); i ++)
                cout<<wordError[i]<<" ";
            cout<<endl<<endl<<"*Utilizando Hamming conseguimos corrigir apenas um bit de erro e detectar dois, caso tenha acontecido inversão de mais de 2 bits a mensagem final ou estará corrigida errada ou não será corrigida."<<endl<<endl;
        }
        else
        {
            cout<<endl<<endl<<"Dois bits de erro detectados!"<<endl;
        }
    }
}

/// A função CRC faz tanto a criação do CRC da mensagem para envio quanto a verificação de erros na mensagem isto é definido pelo objeto verify
void CRC10(vector<bool> &wordCRC, int verify, vector<bool> &word)
{
    vector<bool> protectPolynom = {1,0,0,1};            // Polinômio protetor x³+1 = x³ + 0x² + 0x + 1 = 1 0 0 1
    int erro = 0, k = 3;                                // k = grau do polinômio de pretoção
    vector<bool> CRC = {0,0,0,0};                       // CRC que primeiro será auxiliar do XOR para ao fim virar o CRC

    if(verify==0)
        for(int i = 0; i<protectPolynom.size()-1; i++)  // É feito um shift de k bits
            wordCRC.push_back(0);

    if (verify == 1)                                    // Esse verify == 1 será usado na hora de verificar erros na mensagem
    {
        cout<<endl<<endl<<"A mensagem original enviada foi: "<<endl;
        for(int i = 0; i<word.size(); i++)
            cout<<word[i]<<" ";
    }

    cout<<endl<<"Mensagem recebida Rx + k: "<<endl;
    for(int i = 0; i<wordCRC.size(); i++)
        cout<<wordCRC[i]<<" ";
    cout<<endl;

    for(int i = 0; i<CRC.size();i++)
        CRC[i] = wordCRC[i];                            // objeto CRC recebe os primerios bits da palavra de dados

    int i = CRC.size();                                 // Padrão do tamanho do CRC 4

    while(i<wordCRC.size()+1)                           //começa no i = tamanho do CRC pois essa será a primeira divisão, ou seja primeira parcela da palavra de dados
    {
        cout<<endl<<"CRC ^ polinômio: "<<CRC[0]<<CRC[1]<<CRC[2]<<CRC[3]<<"^"<<protectPolynom[0]<<protectPolynom[1]<<protectPolynom[2]<<protectPolynom[3]<<":"; // Printa a operação
        if(CRC[0] != 0)
        {
            for(int k = 0; k<CRC.size(); k++)
                CRC[k] = CRC[k] ^ protectPolynom[k];    // Faz o XOR bit a bit com o operador de bitwise

            cout<<CRC[0]<<CRC[1]<<CRC[2]<<CRC[3]<<endl; // Resultado da operação
        }
        else
        {
            if(i!=wordCRC.size())                       // Se tiver na última posição não pega o próximo valor
            {
                CRC.erase(CRC.begin());                 // Quando cai nesse else remover o primeira posição que é zero
                CRC.push_back(wordCRC[i]);              // Da divisão faz o "desce 1" ou seja da um insere atrás (push_back) no CRC do próximo valor da palavra de dados (wordCRC)
            }
            i++;
        }
    }

    while(CRC.size()>k)
        CRC.erase(CRC.begin());                         // Se o tamanho do CRC gerado for maior que o número K ele é reduzido a este número

    if (verify == 0)
    {
        int j=0;
        for(int i = wordCRC.size()-CRC.size(); i<wordCRC.size(); i++)
        {
            wordCRC[i] = CRC[j];                        // O CRC é inserido na mensagem
            j++;
        }
    }

    cout<<endl<<endl<<"O CRC gerado foi:"<<endl;
    for(int i = 0; i<CRC.size(); i++)
    {
        cout<<CRC[i]<<" ";
        if(verify == 1)
            erro = erro + CRC[i];                       // Se estiver no modo da mensagem sendo recebida no receptor (verify = 1) é feito a avaliação de erros no CRC
    }

    if (verify == 1)
    {
        if(erro>0)                                      // CRC resultante maior que 0 aponta CRC inválido
            cout<<"CRC inválido! Erro detectado."<<endl;
    }

    if (verify == 0)
    {
        cout<<endl<<endl<<"A mensagem a ser enviada será: "<<endl;
        for(int i = 0; i<wordCRC.size(); i++)
            cout<<wordCRC[i]<<" ";

        cout<<endl<<endl;
    }
    cout<<endl;
}

void lineToVector(string linha, vector<bool>& vec)
{
    stringstream stream;
    int numero;

    stream.str(linha);                                  //facriona a partir dos espaços entre os números na linha

  	while (stream >> numero)                            //operador extract remove do stream para inserir ao objeto número
        vec.push_back(numero);                          //que vai para dentro do vector em seguida

    stream.clear();                                     //limpa o stream
}

void Start(vector<bool> &word, int &mode)
{
    string line;

    cout<<endl<<endl<<"\t\tAplicação de métodos de detecção de erros"<<endl;
    cout<<"\t\t CRC (Cyclic Redundancy Check) e Hamming"<<endl<<endl;
    cout<<"(1) - Utilizar método CRC de 10 bits"<<endl;
    cout<<"(2) - Utilizar método Hamming (12,8) ou (14,10)"<<endl;
    cout<<"Digite de 1 ou 2: "<<endl;
    cin>>mode;

    system("CLS");
    if(mode == 1)
    {
        cin.sync();
        cout<<endl<<endl<<"\t\tAplicação de métodos de detecção de erros"<<endl;
        cout<<"\t\t CRC (Cyclic Redundancy Check) e Hamming"<<endl<<endl;
        cout<<"--->Modo Selecionado CRC"<<endl<<endl<<"    Digite a palavra de dados a ser enviada de 10 bits (Separando por espaços Ex:1 0):"<<endl;
        getline(cin, line);

        lineToVector(line, word);
    }
    if (mode == 2)
    {
        cin.sync();
        cout<<endl<<endl<<"\t\tAplicação de métodos de detecção de erros"<<endl;
        cout<<"\t\t CRC (Cyclic Redundancy Check) e Hamming"<<endl<<endl;
        cout<<"--->Modo Selecionado Hamming (12,8) ou (14,10)"<<endl<<endl<<"    Digite a palavra de dados a ser enviada de 8 ou 10 bits a qual fará Hamming de (12,8) ou (14,10) respectivamente (Separando por espaços Ex:1 0):"<<endl;
        getline(cin, line);

        lineToVector(line, word);
    }
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    string line;
    int mode = 0, L=16, C=4;
    vector<bool> word;
    vector<bool> wordError;

    ///Matriz de Binários - Hamming
    vector<vector<bool>> matrixBinary;                  // Cria objeto MATRIZ que é uma coleção de Vectors do tipo bool
    for(uint32_t i = 0; i < L; ++i)                     // Cria os vetores que serão as Colunas
            matrixBinary.push_back(vector<bool>());

    for(uint32_t i = 0; i < L; ++i)                     // Cria e popula as linhas que passam a "existir"
            for(uint32_t j = 0; j < C; ++j)
                matrixBinary[i].push_back(i);

    for(uint32_t i = 0; i < L; ++i)                     // Cria tabela de binários nessa matriz 0000, 0010 ... 1111
            for(uint32_t j = 0; j < C; ++j)             // Será utilizado para definição dos bits de paridade
                matrixBinary[i][C - j - 1] = ((i >> j) & 1) == 1;

    while(true)
    {
        Start(word, mode);
        /// CRC
        if(mode == 1)
        {
            mode = 0;                                  // Modo = 0 fazer a mensagem para enviar
            CRC10(word, mode, word);

            cin.sync();
            cout<<endl<<"Deseja simular erros na mansagem?(1 = Sim/ 2 = não)"<<endl;
            cin>>mode;

            if(mode == 1)
            {
                cin.sync();
                cout<<"Digite a palavra de dados com erro a ser enviada de 10 bits com os bits de CRC (Separando por espaços Ex:1 0):"<<endl;
                getline(cin, line);

                lineToVector(line, wordError);
            }
            else
            {
                wordError = word;                      // Caso não se deseje inserir erros
            }

            mode = 1;                                  // Modo = 1 receptor recebe a mensagem e verifica erro
            CRC10(wordError, mode, word);
        }
        /// Hamming
        if(mode == 2)
        {
            vector<int> parityBit;                      // Vector para bits de paridade cada posição da 0 a 3 representa respecitvamente: p1, p2, p4 e p8.

            for(int i = 0; i < PARITYMAX; i++ )
                parityBit.push_back(0);                 // Criação do vector de bits de paridade

            for(int i = 0; i<4; i++)                    // Cria as posições do bit de paridade por exemplo p1 é criado um 0 na primeira posição do vetor,
                word.insert(word.begin()+pow(2,i)-1,0); // após p1, para p2 é criado um 0 na segunda posição e assim por diante

            hamming14_10(matrixBinary, word, parityBit);

            for(int i = 0; i<PARITYMAX; i++)
                word[pow(2,i)-1] = parityBit[i];                // Insere os bits de paridade calculado na palavra

            cout<<endl<<"Os bits de paridade p1, p2, p4 e p8 são respectivamente: "<<endl;
            for(int i = 0; i<PARITYMAX; i++)
                cout<<parityBit[i]<<" ";

            cout<<endl<<endl<<"A mensagem a ser enviada será: "<<endl;
            for(int i = 0; i<word.size(); i ++)
                cout<<word[i]<<" ";
            cout<<endl;

            doubleBitInsert(word);                      // Faz um XOR de todos os bits da palavra de dados e insere a frente dessa palavra

            cin.sync();
            cout<<endl<<"Deseja simular erros na mansagem?(1 = Sim/ 2 = não)"<<endl;
            cin>>mode;

            if(mode == 1)
            {
                cin.sync();
                cout<<"Digite a palavra de dados com erro a ser enviada com os bits de paridade e o bit ''P'' (Separando por espaços Ex:1 0):"<<endl;
                getline(cin, line);

                lineToVector(line, wordError);
            }
            else
            {
                wordError=word;
            }

            for(int i = 0; i <PARITYMAX; i++)
                parityBit[i] = 0;                       // Define o objeto parityBit para 0 em todas as posições

            hamming14_10Verify(matrixBinary, word, parityBit, wordError);

            parityBit.clear();
        }
        system("pause");
        mode = 0;
        word.clear();
        wordError.clear();
        system("CLS");
    }
    return 0;
}
