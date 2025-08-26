def controllo(valore):                                                                           #Definisco il controllo per i numeri
    flag = True                                                                                  #Inizializzo la variabile flag assegnando il valore True
    while flag:                                                                                  #Ciclo while
        if valore.isdigit():                                                                     #Controllo che il valore inserito sia un numero
            valoreIntero = int(valore)                                                           #Effettuo il cast della variabile in intera 
            flag = False                                                                         #Imposto la variabile flag a False per poter uscire dal ciclo while
        else:
            valore = input("Errore\nInserire un numero valido: ")                                #In caso i valori non siano numeri stampo un messaggio di errore e chiedo un nuovo input
    return valoreIntero

def verifica(testo):                                                                             #Definisco il controllo per le stringe
    flag = True                                                                                  #Imposto la variabile flag come True
    while flag:                                                                                  #Ciclo while
        if testo.replace("" , ""):                                                               #Verifica input vuoto
            testoVerificato = str(testo)                                                         #Effettuo il cast della variabile in stringa
            flag = False                                                                         #Imposto la variabile flag a False per poter uscire dal ciclo while
        else:
            testo = input("Errore\nInserire un valore valido: ")                                 #In caso i valori non siano stringhe stampo un messaggio di errore e chiedo un nuovo input
    return testoVerificato

def verData(data):                                                                               #Definisco il controllo per la data
    flag = True                                                                                  #Imposto la variabile flag come True
    while flag:                                                                                  #Ciclo while
        parti = data.split("-")                                                                  #Divido la variabile dove presente "-"
        if len(parti) == 3:                                                                      #Verifico che le parti che compongono l'input siano 3
            anno , mese , giorno = parti[0] , parti[1] , parti[2]                                #Suddivido le 3 parti in variabili distinte
            if anno.isdigit() and mese.isdigit() and giorno.isdigit():                           #Verifico che le 3 componenti siano dei numeri
                anno = int(parti[0])                                                             #Effettuo un cast della variabile anno
                mese = int(parti[1])                                                             #Effettuo un cast della variabile mese
                giorno = int(parti[2])                                                           #Effettuo un cast della variabile giorno
                if 1 <= mese <= 12 and 1 <= giorno <= 31:                                        #Verifico che la variabile mese e giorno abbiano dei valori compresi fra quelli impostati
                    flag = False                                                                 #Imposto la variabile flag come False per uscire dal ciclo while
                else:
                    data = input("Errore\nInserire un numero valido: ")                          #Stampo un messaggio di errore e chiedo di inserire un nuovo input
            else:
                data = input("Errore\nInserire un numero valido: ")                              #Stampo un messaggio di errore e chiedo di inserire un nuovo input
        else:
            data = input("Errore\nInserire un numero valido: ")                                  #Stampo un messaggio di errore e chiedo di inserire un nuovo input
    return data

def menu(menuDigit):                                                                             #Definisco la funzione menu
    print(menuDigit)                                                                             #Stampo il menu

def main():                                                                                      #Definisco il main
    flag = True                                                                                  #Inizializzo la variabile flag assegnando il valore True
    while flag:                                                                                  #Ciclo while
        menuDigit = "\n1.Aggiungi una prenotazione\n2.Visualizza tutte le prenotazioni\n3.Cerca una prenotazione\n4.Modifica la destinazione di una prenotazione\n5.Annulla le prenotazioni\n6.Esci\n" #Inizializzo la variabile menuDigit e gli assegno una stringa
        menu(menuDigit)                                                                          #Inserisco la variabile menuDigit nel def menu
        numero = input("Inserire una selezione: ")                                               #Chiedo all'utente un input
        numeroVerificato = controllo(numero)                                                     #Verifico l'input inserito
        if numeroVerificato == 6:                                                                #Controllo se l'input inserito, verificato è uguale a 6
            flag = False                                                                         #Imposto la variabile flag come False per uscire dal ciclo while
        else:
            selezione(numeroVerificato)                                                          #Inserisco la variabile numeroVerificato nel def selezione
    
dictionary = {}                                                                                  #Creo un dizionario
def digitUno():                                                                                  #Definisco la funzione digitUno
    global dictionary                                                                            ##                                                           ##
    global lista                                                                                 # Globalizzo le variabili dictionary , lista , dataVerificata #
    global dataVerificata                                                                        ##                                                           ##
    nome = input("\nInserire il nome e il cognome: ")                                            #Chiedo all'utente di inserire il nome e il cognome
    nomeVerificato = verifica(nome)                                                              #Verifico l'input inserito
    nomeVerificatoLower = nomeVerificato.lower()                                                 #Inizializzo la variabile nomeVerificatoLower e gli assegno il valore dell'input del nome verificato tutto minuscolo
    destinazione = input("Inserire la destinazione: ")                                           #Chiedo all'utente di inserire la destinazione
    destinazioneVerificata = verifica(destinazione)                                              #Verifico l'input inserito
    destinazioneVerificataLower = destinazioneVerificata.lower()                                 #Inizializzo la variabile destinazioneVerificataLower e gli assegno il valore dell'input della destinazione verificata tutto minuscolo
    data = input("Inserire la data del volo in formato (YYYY-MM-DD): ")                          #Chiedo all'utente di inserire la data
    dataVerificata = verData(data)                                                               #Verifico l'input inserito
    lista = [(destinazioneVerificataLower , dataVerificata)]                                     #Creo una lista e inserisco all'interno le variabili destinazioneVerificataLower , dataVerificata
    if nomeVerificatoLower in dictionary:                                                        #Verifico che il nome verificato inserito sia presente nel dizionario
        dictionary[nomeVerificatoLower] += lista                                                 #Aggiorno la lista
    else:
        dictionary[nomeVerificatoLower] = lista                                                  #Inserisco il nome verificato (chiave) nel dizionario e assegno la lista come valore
    print("\nPrenotazione aggiunta per" , nomeVerificato + ", destinazione:" , destinazioneVerificata + " , data:" , dataVerificata) #Stampo l'avvenuta registrazione
    
def digitDue():                                                                                  #Definisco la funzione digitDue
    print("\nPrenotazioni:")                                                                     #Stampo il titolo
    for nome in dictionary:                                                                      ##                                                                                                                                                                                                      ##                 
        print(nome + ":" , end = "")                                                             #                                                                                                                                                                                                        #                            
        prenotazione = dictionary[nome]                                                          #Stampo tutti i dati presenti nel dizionario; prendo il valore associato al nome (chiave) e lo salvo su prenotazione, con un ciclo for vado a prendere e stampare tutte le destinazioni [0] e le date [1]#
        for i in range(len(prenotazione)):                                                       #                                                                                                                                                                                                        #           
            print("" , prenotazione[i][0] , prenotazione[i][1])                                  ##                                                                                                                                                                                                      ##        

def digitTre():                                                                                  #Definisco la funzione digitTre
    cerca = input("Effettuare la ricerca per nome: ")                                            #Chiedo all'utente di inserire un nome per eseguire la ricerca
    cercaVerificato = verifica(cerca)                                                            #Verifico l'input inserito
    cercaVerificatoLower = cercaVerificato.lower()                                               #Inizializzo la variabile cercaVerificatoLower e gli assegno il valore dell'input della ricerca verificata tutto minuscolo
    if cercaVerificatoLower in dictionary:                                                       #Verifico se il nome inserito nella ricerca è presente nel dizionario
        prenotazioni = dictionary[cercaVerificatoLower]                                          #Estrapolo dal dizionario le prenotazioni cercando per nome e salvo i dati nella variabile prenotazioni
        for i in range(len(prenotazioni)):                                                       #Stampo tutte le prenotazioni riferite ad un certo utente
                print(i + 1  , prenotazioni[i][0] , "-" , prenotazioni[i][1])                    ##
    else:
        print("\nPrenotazione non effettuata")                                                   #Stampo un messaggio di errore

def digitQuattro():                                                                              #Definisco la funzione digitQuattro
    nome = input("Inserire il nome del passeggero: ")                                            #Chiedo all'utente di inserire il nome del passeggero
    nomeVerificato = verifica(nome)                                                              #Effettuo una verifica sull'input inserito
    nomeVerificatoLower = nomeVerificato.lower()                                                 #Inizializzo la variabile nomeVerificatoLower e assegno il valore del nome del passeggero verificato e trasformato in minuscolo
    flag = True                                                                                  #Inizializzo la variabile flag assegnando il valore True
    while flag:                                                                                  #Ciclo while
        if nomeVerificatoLower in dictionary:                                                    #Verifico che il nome sia presente nel dizionario
            prenotazioni = dictionary[nomeVerificatoLower]                                       #Estrapolo dal dizionario le prenotazioni discriminando per nome e salvo i dati nella variabile prenotazioni
            for i in range(len(prenotazioni)):                                                   #Stampo tutte le prenotazioni riferite ad un certo utente
                print(i + 1  , prenotazioni[i][0] , "-" , prenotazioni[i][1])                    ##
            select = input("Selezionare la prenotazione da modificare: ")                        #Chiedo all'utente di selezionare una prenotazione che desidera modificare 
            selectVerificato = controllo(select) - 1                                             #Inserisco la variabile select in controllo
            if 0 <= selectVerificato < len(prenotazioni):                                        #Verifico che la selezione sia nel range delle prenotazioni esistenti
                dest = input("Inserire la nuova destinazione: ")                                 #Chiedo all'utente di inserire la nuova destinazione
                destVerificata = verifica(dest)                                                  #Inserisco la variabile dest in verifica
                destVerificataLower = destVerificata.lower()                                     #Inizializzo la variabile destVerificataLower e assegno il valore della nuova destinazione verificata e trasformata in minuscolo
                data = prenotazioni[selectVerificato][1]                                         #Inizializzo la variabil data assegnando il valore della data estrapolato dal dizionario discriminando per destinazione
                prenotazioni[selectVerificato] = (destVerificataLower , data)                    #Effettuo il cambio di destinazione andando a modificare il dizionario
                print("\nDestinazione modificata con successo")                                  #Stampo l'avvenuta modifica
                flag = False                                                                     #Assegno alla variabile flag il valore di False per uscire dal ciclo while
            else:
                selectVerificato = input("Inserire un valore valido: ")                          #Stampo un messaggio di errore e chiedo di inserire un valore valido
        else:
            print("\nPrenotazione inesistente")                                                  #Stampo un messaggio di errore
            flag = False                                                                         #Assegno alla variabile flag il valore di False per uscire dal ciclo while

def digitCinque():                                                                               #Definisco la funzione digitCinque
    nome = input("Inserire il nome del passeggero: ")                                            #Chiedo all'utente di inserire il nome del passeggerp
    nomeVerificato = verifica(nome)                                                              #Verifico l'input inserito
    nomeVerificatoLower = nomeVerificato.lower()                                                 #Inizializzo la variabile nomeVerificatoLower e assegno il valore del nome verificato e trasformato in minuscolo
    if nomeVerificatoLower in dictionary:                                                        #Verifico se il nome è presente nel dizionario
        dictionary.pop(nomeVerificatoLower)                                                      #Rimuovo dal dizionario le prenotazioni dell'utente selezionato
        print("\nPrenotazioni cancellate con successo")                                          #Stampo l'avvenuta cancellazione
    else:
        print("\nPrenotazione insesistente")                                                     #Stampo un messaggio di errore

def selezione(numeroVerificato):                                                                 ##                                           Definisco la funzione selezione                                            ##
    if numeroVerificato == 1:                                                                    #                                                                                                                        #
        digitUno()                                                                               #                                                                                                                        #
    elif numeroVerificato == 2:                                                                  #                                                                                                                        #
        digitDue()                                                                               #                                                                                                                        #
    elif numeroVerificato == 3:                                                                  #                                                                                                                        #
        digitTre()                                                                               #Vado a verificare l'input dell'utente e richiamo le corrispondenti definizioni, altrimenti stampo un messaggio di errore#
    elif numeroVerificato == 4:                                                                  #                                                                                                                        #
        digitQuattro()                                                                           #                                                                                                                        #  
    elif numeroVerificato == 5:                                                                  #                                                                                                                        #
        digitCinque()                                                                            #                                                                                                                        #
    else:                                                                                        #                                                                                                                        #
        print("\nSelezione non valida")                                                          ##                                                                                                                      ##

if __name__ == "__main__":                                                                       #Main
    main()                                                                                       #Main