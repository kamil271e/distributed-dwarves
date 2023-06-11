# Algorytm (krasnale)

- krasnal czeka w pętli na zlecenie
- skansen generuje zlecenie
- skansen wysyła zlecenie do wszystkich krasnali
- każdy z krasnali odbiera zlecenie i wysyła REQ do innych krasnali
- krasnale odbierają REQ dot. zlecenia od swoich braci:
    - jeśli krasnal nie ubiega się o to samo zlecenie wysyła ACK
    - jeśli krasnal ubiega się o to zlecenie, ale ma niższy priorytet to wysyła ACK i porzuca zadanie
    - jeśli krasnal ubiega się o to zlecenie i ma wyższy priorytet to nie wysyła ACK
- krasnal, który otrzymał ACK od wszystkich braci przechodzi do zajmowania portalu
- krasnal, który jest 2. w kolejności decyduje o kolejności kolejnych zleceń (wg tego w jakiej kolejności do niego przyjdą) i wysyła o nich informację do krasnoludów wg kolejności REQ, które one wysyłały do siebie wcześniej
- krasnal ten dzieli się na 2 wątki - 1. odpowiada ACK na wszystkie REQ dot. innych zleceń; 2. idzie dalej
- krasnale odbierają REQ dot. portali od swoich braci:
    - jeśli krasnal nie ubiega się o portal to wysyła ACK
    - jeśli krasnal ubiega się o portal, ale ma niższy priorytet to wysyła ACK
    - jeśli krasnal ubiega się o portal i ma wyższy priorytet to dodaje proces, który wysłał REQ do listy (wyśle mu ACK po wyjściu z sekcji krytycznej)
- krasnal, który otrzymał ACK od chociaż (ilość_krasnoludów-1-X) braci przechodzi dalej (X to ilość portali)
- krasnal, który ma zlecenie i portal wykonuje zadanie
- krasnal, który wykonał zlecenie zwalnia portal (wychodzi z sekcji krytycznej) i wysyła ACK do krasnali, które nagromadziły się na jego liście
- krasnal wraca do oczekiwania na zlecenie

-------------------------------------------------------


osobne programy dla skansenow i krasnali, liczba podawana podczas kompilacji - makro
TODO: określanie kolejności kolejnych zleceń
TODO: sa sytuacje ze wiecej niz jeden krasnal wykonuje dana fuchę