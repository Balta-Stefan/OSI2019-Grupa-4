#include "backEnd.h"

bool authenticate(std::string sessionID)
{
    //provjeriti da li globalni vektor zvan "sessions", koristiti for(auto &counter : sessions) petlju
        //sadrzi sessionID pa vratiti odgovarajuci bool
}

bool isAdmin(std::string sessionID)
{
    //isto kao funkcija authenticate, provjeriti da li je proslijedjeni sessionID od administratora
    //povratna vrijednost je bool i false je za neuspjesno logovanje, true u protivnom
}

std::string login(std::string username, std::string password)
{
    //provjeriti fajl "korisnici.txt"
    //ako postoji takav korisnik, napraviti struct session i staviti u globalni vektor sessions
}

std::string eventComment(std::string comment, std::string eventName)
{
    //staviti komentar comment u dogadjaj sa nazivom eventName
    //voditi racuna o formatu u fajlu komentari.txt
    //povratna vrijednost je poruka o uspjesnosti, koristiti neku globalnu promjenljivu
}

std::string banUser(std::string userName)
{
    //u fajlu korisnici.txt pronaci korisnika userName
    //promijeniti mu zadnji element u redu (broj) u nulu
    //povratna vrijednost je poruka o uspjesnosti, koristiti neku globalnu promjenljivu
}

struct quiz getQuizInfo()
{
  //POPUNITI STRUKTURU STRUCT QUIZ
  //ovo je funkcija koja pribavlja podatke o kvizu za administratora
  //vratiti tu popunjenu strukturu
}

void editQuiz(struct quiz newQuiz)
{
    //FUNKCIJA ZA ADMINISTRATORA, KORISTI SE ZA PROMJENU KVIZA
}

std::vector<std::string> getCategories()
{
    //FUNKCIJA KOJA PRIBAVLJA KATEGORIJE ZA ADMINISTRATORA
    //NALAZE SE U FAJLU kategorije.txt
    //POPUNITI VEKTOR STRINGOVA I VRATITI GA
}

void changeCategories(std::vector<std::string> newCategories)
{
    //ADMINISTRATORKSA FUNKCIJA KOJA MIJENJA KATEGORIJE
    //SADRZAJ VEKTORA newCategories UPISATI U FAJL kategorije.txt, CITAVOG GA IZMIJENITI
}

struct quiz4Players getQuestions4Player()
{
    //FUNKCIJA KOJA PRIBAVLJA PITANJA I PONUDJENE ODGOVORE ZA IGRACE
}

int checkPlayersAnswers(struct quiz)
{
    //FUNKCIJA KOJA PROVJERAVA KOLIKO JE ODGOVORA ISPRAVNO
    //VRATITI BROJ ISPRAVNIH ODGOVORA
}

std::string removeComment(std::string commentID, std::string eventName)
{
    //ADMINISTRATORSKA FUNKCIJA ZA UKLANJANJE KOMENTARA
    //U FOLDERU eventName SE NALAZI FAJL komentari.txt
    //UKLONITI KOMENTAR U REDU SA commentID
}
