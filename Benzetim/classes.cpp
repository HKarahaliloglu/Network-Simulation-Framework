#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <memory>

using namespace std;

enum  EVENT_TYPE { MESSAGE,NEW_MESSAGE, LINK_FREE, ROUTING,TRY_AGAIN };
#define MAX_FIFO 10

//enum port_turu = { Nort };
// Olay sýnýfý
class Olay {
private:

	int olayZamani;
	EVENT_TYPE olayTuru;
	int hedefComputeId;
	int baslangýcComputeId;
	int bulunulanSwitchId;
	
	

public:
	int sonrakiPortID =99;
	int oncekiswitchId =998;
	int portID;
	int sonrakiswitchId =997;
	int hedefKontrol;
	int olayReceived = 0;
	Olay(const int olayZamani1, EVENT_TYPE olayTuru1, int baslangýcComputeId1, int hedefComputeId1, int bulunulanSwitchId1) {
		olayZamani = olayZamani1;
		olayTuru = olayTuru1;
		baslangýcComputeId = baslangýcComputeId1;
		hedefComputeId = hedefComputeId1;
		bulunulanSwitchId = bulunulanSwitchId1;
	

	}
		

	//get fonksyonlarý
	EVENT_TYPE getOlayTuru() const { return olayTuru; }
	int getOlayZamani() const { return olayZamani; }
	int getHedefComputeId() const { return hedefComputeId; }
	int getBaslangýcComputeId() const { return baslangýcComputeId; }
	int getBulunulanSwitchId() const { return bulunulanSwitchId; }

	// Set fonksiyonlarý 
	void setOlayTuru(const EVENT_TYPE yeniOlayTuru) { olayTuru = yeniOlayTuru; }
	void setOlayZamani(int yeniZaman) { olayZamani = yeniZaman; }
	void setHedefComputeId(int yeniHedefComputeId) { hedefComputeId = yeniHedefComputeId; }
	void setIlkComputeId(int yeniIlkComputeId) { baslangýcComputeId = yeniIlkComputeId; }
	void setBulunulanSwitchId(int yeniBulunulanSwitchId) { bulunulanSwitchId = yeniBulunulanSwitchId; }
};





class LimitedQueue : public queue<Olay> {
public:
	using queue<Olay>::queue;  // Kuyruk yapýsýnýn kurucularýný devral
	unsigned full =MAX_FIFO;
	void push(const Olay& value) {
		if (this->size() >= full) {
			
			return;  // Kuyruk doluysa ekleme yapma
		}

		queue<Olay>::push(value);  // Yeni elemaný ekle
	}
};

class Port {

public:
	bool linkfree = true;
	int port_Received = 0;
	int credit = 10;
	//queue<Olay> HW_fifo_send;
	LimitedQueue HW_fifo_send;
	LimitedQueue HW_fifo_recv;
	Port* kime;

	int getReceived() const { return port_Received; }
	int getCredit() const { return credit; }
	void setReceived(int yeniReceived) { port_Received = yeniReceived; }
	void setCredit(int yeniCredit) { credit = yeniCredit; }

	Port() = default;
};

// Node sýnýfý
class Node {
public:
	int id;
	string name;
	


	Node(int id, string name) : id(id), name(name) {}
};

// Switch sýnýfý (Node sýnýfýndan miras alýr)
class Switch : public Node {
public:
	static const int NUM_PORTS = 5; // Toplam port sayýsý
	int type = 1;
	Port ports[NUM_PORTS]; // 0=>Compute,1=>kuzey, 2=>doðu, 3=>güney, 4=>batý
	
	Switch(int id, string name) : Node(id, name) {

	}
};

// Ýþlemci sýnýfý (Node sýnýfýndan miras alýr)
class Compute : public Node {
public:
	int type = 0;
	Port port_compute;

	Compute(int id, string name) : Node(id, name) {}
};