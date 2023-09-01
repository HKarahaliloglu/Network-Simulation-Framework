#include <iostream>
#include "classes.cpp"
#include <string>
#include <queue>
#include <functional>
#include <random>
#include <ctime>

using namespace std;
int simulation_time = 0;
int linkSuresi = 1;



// �ki boyutlu torus �zerinde sonraki eleman�, mesaj�n ��kt��� portu ve girece�i portu bulma
vector<int> findTorusPath(int start, int target) {
	vector<int> path;

	int numRows = 3;
	int numCols = 4;

	int startX = start / numCols;
	int startY = start % numCols;
	int targetX = target / numCols;
	int targetY = target % numCols;

	int deltaX = (targetX - startX + numRows) % numRows;
	int deltaY = (targetY - startY + numCols) % numCols;

	if (deltaX > 0) {
		startX = (startX + 1) % numRows;
		path.push_back(startX * numCols + startY);  // bir sonraki switch id
		path.push_back(1); // recv port no
		path.push_back(3); // send port no
	}
	else if(deltaY > 0) {
		startY = (startY + 1) % numCols;
		path.push_back(startX * numCols + startY); // bir sonraki switch id
		path.push_back(4);  // recv port no
		path.push_back(2); // send port no
	}
	else {

		cout << "Hedef Compute geliniyor..." << endl;		
	}
	return path;
	}
	
// Kuyruktaki olaylar� ekrana yazd�ran fonksiyon
void kuyrukGoster(const priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>>& sorted_queue) {
	priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>> copy_queue = sorted_queue;

	while (!copy_queue.empty()) {
		const Olay& front_element = copy_queue.top();
		EVENT_TYPE olayTuru = front_element.getOlayTuru();
		int time = front_element.getOlayZamani();
		int a = front_element.getBulunulanSwitchId();

		//cout << "------------" << endl;
		cout << "=> Olay Zamani: " << time << "  /  " << "Olay T�r�: " << olayTuru << "  /  " << "Bulunnulan Switch ID  :" << a << endl;
		copy_queue.pop();
	}
}

// Kuyrukta bekleyen bir olay� kullanan fonksiyon
bool kuyrukOlayKullan(priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>>& sorted_queue, vector<Switch>& switches, vector<Compute>& computes) {
	if (sorted_queue.empty()) {
		cout << "Kuyrukta Bekleyen Olay Bulunmuyor" << endl;
		return false; // D�ng�y� durdur
	}

		Olay ilkOlay = sorted_queue.top(); // �lk olaya eri�im
		EVENT_TYPE olayTuru = ilkOlay.getOlayTuru();
		int olayZamani = ilkOlay.getOlayZamani();
		int baslang�cNodeId = ilkOlay.getBaslang�cComputeId();
		int hedefComputeId = ilkOlay.getHedefComputeId();
		int bulunulanSwitchId = ilkOlay.getBulunulanSwitchId();
		Compute& baslang�cCompute = computes[baslang�cNodeId];  // ilkCompute nesnesine eri�ebilirsiniz
		Compute& hedefCompute = computes[hedefComputeId];  // HedefCompute nesnesine eri�ebilirsiniz
		Switch& bulunulanNode = switches[bulunulanSwitchId];  // BulunulanSwitch nesnesine eri�ebilirsiniz

		simulation_time = olayZamani;
		cout << "Kuruktan ��kan :" << simulation_time << olayTuru << endl;
		cout << "G�ncel S�mulation Zaman� ==>" << olayZamani << endl;


		switch (olayTuru) {
		case MESSAGE:
		{

			cout << olayZamani << " olay zamanl� message olay� �al��t�" << endl;

			if (baslang�cCompute.id == bulunulanSwitchId) {

				bulunulanNode.ports[0].HW_fifo_recv.push(ilkOlay);
				cout << bulunulanSwitchId << " Numaral� switchin compute portunun HW_fifo_ recv ine olay eklendi" << endl;
				// Kuyruktaki eleman say�s�n� bulma
				int itemCount = bulunulanNode.ports[0].HW_fifo_recv.size();
				//cout << " ba�lang��daki ilk switchin recv Kuyruktaki eleman --sayisi: " << itemCount << std::endl;
			}

			else if (hedefComputeId == ilkOlay.hedefKontrol) {
				hedefCompute.port_compute.HW_fifo_recv.push(ilkOlay);
				cout << hedefComputeId << " Numaral� compute �n compute portunun HW_fifo_ recv ine mesaj eklendi ve mesaj al�n�p t�ketildi." << endl;
				hedefCompute.port_compute.HW_fifo_recv.pop();
			}

			else if (bulunulanNode.type == 1) {


				bulunulanNode.ports[ilkOlay.sonrakiPortID].HW_fifo_recv.push(ilkOlay);
				cout << bulunulanNode.id << "Numaral� switchin " << ilkOlay.sonrakiPortID << "No lu portuna mesaj eklendi" << endl;
			}
			ilkOlay.setOlayTuru(ROUTING);
			//Olay routing(simulation_time, "routing", baslang�cNodeId, hedefComputeId, bulunulanSwitchId);		
			cout << bulunulanSwitchId << " No lu switch i�in Routing olay� olu�turuldu ve global kuyru�a eklendi" << endl;
			sorted_queue.push(ilkOlay);
		}

		break;
		
		case NEW_MESSAGE:
		{
			cout << "newmessage olay� �al��t�" << endl;

			if (baslang�cCompute.port_compute.HW_fifo_send.size() == baslang�cCompute.port_compute.HW_fifo_send.full) {
				//Olay tryAgain(simulation_time, "tryagain", baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
				//sorted_queue.push(tryAgain);
				ilkOlay.setOlayZamani(olayZamani + 1);
				sorted_queue.push(ilkOlay);
				cout << baslang�cNodeId << " No lu Compute �n send Fifosu dolu oldu�u i�in newmessage olay� �al��mad�, new message �al��aca�� s�re 1 art�r�l�p tekrar globl kuyru�a eklendi." << endl;

			}
			else {

				baslang�cCompute.port_compute.HW_fifo_send.push(ilkOlay);
				int itemCount = baslang�cCompute.port_compute.HW_fifo_send.size();
				cout << baslang�cNodeId << " Nolu compute node un HW_fifo_send ine yeni olay eklendi! " << endl;
				cout << baslang�cNodeId << " No lu computenin send fifosundaki eleman sayisi: " << itemCount << endl;

				if (baslang�cCompute.port_compute.linkfree == true && baslang�cCompute.port_compute.credit > 0) {
					cout << baslang�cNodeId << " numaral� compute node un linki free ve credisi kontrol edildi" << endl;
					cout << baslang�cNodeId << " Nolu compute node un HW_fifo_send inde bekleyen ilk olay ��kar�ld� " << endl;
					baslang�cCompute.port_compute.HW_fifo_send.pop();
					baslang�cCompute.port_compute.credit = baslang�cCompute.port_compute.credit - 1;
					cout << baslang�cNodeId << " numaral� compute node un credisi 1 azalt�ld� , yeni credi de�eri => " << baslang�cCompute.port_compute.credit << endl;
					baslang�cCompute.port_compute.linkfree = false;
					cout << baslang�cNodeId << " numaral� compute node un linki false yap�ld�" << endl;
					//cout << ilkOlay.olayReceived << endl;
					//cout << baslang�cCompute.port_compute.port_Received << endl;
					//ilkOlay.olayReceived = baslang�cCompute.port_compute.port_Received;
					//baslang�cCompute.port_compute.port_Received = 0;
					ilkOlay.setOlayTuru(MESSAGE);
					ilkOlay.setOlayZamani(simulation_time + linkSuresi);
					cout << "Olay�n t�r� mesaja �evrildi , simulasyon zaman� +link yap�ld� ve global kuyru�a eklendi" << endl;
					sorted_queue.push(ilkOlay);

					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
					//link_free.sonrakiswitchId = bulunulanSwitchId;
					link_free.oncekiswitchId = bulunulanSwitchId;
					//link_free.portID = NULL;
					cout << "linkfree olay� olu�turuldu, time � + link s�resi �eklinde ayarland� ve global kuyru�a eklendi" << endl;
					sorted_queue.push(link_free);
				}
			}
		}
		break;

		case LINK_FREE:
		{
			cout << bulunulanSwitchId << " No lu switch i�in link_free olay� �al��t�" << endl;

			if (ilkOlay.oncekiswitchId == baslang�cCompute.id) {
				cout << "link free olay� " << baslang�cNodeId << " No lu compute i�in �al��t� ve send linki tekrar TRUE hale getirildi" << endl;
				baslang�cCompute.port_compute.linkfree = true;

				if (!baslang�cCompute.port_compute.HW_fifo_send.empty() && baslang�cCompute.port_compute.credit > 0) {
					Olay& event = baslang�cCompute.port_compute.HW_fifo_send.front();
					cout << baslang�cNodeId << " No lu compute �n send inde bekleyen olay var ve kredisi 0 da b�y�k oldu�u i�in , kredi miktar� =>> " << baslang�cCompute.port_compute.credit << endl;
					baslang�cCompute.port_compute.HW_fifo_send.pop();
					cout << baslang�cNodeId << " Nolu compute node un HW_fifo_send inde bekleyen ilk olay ��kar�ld� " << endl;
					baslang�cCompute.port_compute.credit = baslang�cCompute.port_compute.credit - 1;
					cout << baslang�cNodeId << " numaral� compute node un credisi 1 azalt�ld� , yeni credi de�eri => " << baslang�cCompute.port_compute.credit << endl;
					baslang�cCompute.port_compute.linkfree = false;
					cout << baslang�cNodeId << " numaral� compute node un linki false yap�ld�" << endl;



					event.setOlayTuru(MESSAGE);
					event.setOlayZamani(simulation_time + linkSuresi);
					cout << "Olay�n t�r� mesaja �evrildi , simulasyon zaman� +link yap�ld� ve global kuyru�a eklendi" << endl;
					sorted_queue.push(event);
					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
					link_free.oncekiswitchId = baslang�cNodeId;
					//link_free.portID = NULL;
					sorted_queue.push(link_free);
					cout << "linkfree olay� olu�turuldu, time � + link s�resi �eklinde ayarland� ve global kuyru�a eklendi" << endl;

					//Olay routing(simulation_time, "routing", baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
					//sorted_queue.push(routing);
				}
			}
			else if (bulunulanSwitchId == hedefComputeId) {
				bulunulanNode.ports[0].linkfree = true;
				cout << "link free olay� " << hedefComputeId << " No lu switch i�in �al��t� ve send linki tekrar TRUE hale getirildi" << endl;
			}

			else {
				Port* newPort = nullptr;

				bulunulanNode.ports[ilkOlay.portID].linkfree = true;

				cout << ilkOlay.portID << endl;



				newPort = &bulunulanNode.ports[ilkOlay.portID];

				if (!newPort->HW_fifo_send.empty() && newPort->credit > 0) {
					newPort->HW_fifo_send.pop();

					newPort->credit = newPort->credit - 1;
					newPort->linkfree = false;


					int start = ilkOlay.getBulunulanSwitchId();
					int target = ilkOlay.getHedefComputeId();

					vector<int> path = findTorusPath(start, target);
					cout << "Sonraki switch ID si: " << path[0] << endl;
					cout << "Sonraki switchin recv port numaras�: " << path[1] << endl;
					cout << "Bulunulan switchin send port numaras�: " << path[2] << endl;

					ilkOlay.setOlayTuru(MESSAGE);
					ilkOlay.setOlayZamani(simulation_time + linkSuresi);
					ilkOlay.setBulunulanSwitchId(path[0]);
					ilkOlay.sonrakiPortID = path[1];

					sorted_queue.push(ilkOlay);
					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
					link_free.portID = path[2];
					sorted_queue.push(link_free);
				}
			}
		}
		break;
		case ROUTING:
		{
			vector<int> portIndices = { 0, 1, 2, 3, 4 };
			cout << bulunulanSwitchId << " No lu switch i�in routing olay� �al��t�" << endl;
			for (int portIndex : portIndices) {
				if (!switches[bulunulanSwitchId].ports[portIndex].HW_fifo_recv.empty()) {
					cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portunun recv inde bulunan olay i�in routing yap�lacak" << endl;
					Olay& event = switches[bulunulanSwitchId].ports[portIndex].HW_fifo_recv.front();


					int start = event.getBulunulanSwitchId();
					int target = event.getHedefComputeId();

					vector<int> path = findTorusPath(start, target);


					if (path.size() >= 1) {
						cout << "Sonraki switch ID si: " << path[0] << endl;
						cout << "Sonraki switchin recv port numaras�: " << path[1] << endl;
						cout << "Bulunulan switchin send port numaras�: " << path[2] << endl;
						int targetPortIndex = path[2];

						if (targetPortIndex != -1) {

							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunua mesaj ta��nacak" << endl;
							LimitedQueue& recvQueue = switches[event.getBulunulanSwitchId()].ports[portIndex].HW_fifo_recv;
							LimitedQueue& sendQueue = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].HW_fifo_send;

							if (sendQueue.size() < sendQueue.full) {
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunda yer var m� diye kontrol edildi." << endl;
								cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portundan mesaj ��kar�ld�" << endl;
								recvQueue.pop();
								//cout << switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit << endl;
								switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit = switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit + 1; //--------------------------------------------------------------------------------------------
								//cout << switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit << endl;
								//event.olayReceived = event.olayReceived + 1; // napt���n� tam anlamad�m
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portuna ��kan mesaj eklendi." << endl;
								sendQueue.push(event);

								if (switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit > 0 && switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree == true) {

									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FREE mi ve credisi yeterlimi diye bak�ld�. Credisi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;
									sendQueue.pop();
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portundan mesaj ��kar�ld�" << endl;
									switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree = false;
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FALSE yap�ld�" << endl;
									switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit - 1;
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun credisi 1 d���r�ld�, G�ncel Credi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;

									event.setOlayZamani(linkSuresi + simulation_time);
									//receivde  =0 kodunu ekle
									cout << "Olay�n t�r� message yap�ld�, zaman� + link s�resi yap�ld�, bulunaca�� switch numaras� " << path[0] << " yap�ld�. �ncekiswitch no suda" << bulunulanSwitchId << " olarak belirlendi ve global kuyru�a eklendi " << endl;

									event.setBulunulanSwitchId(path[0]);
									event.setOlayTuru(MESSAGE);
									event.sonrakiPortID = path[1];

									sorted_queue.push(event);
									cout << "+ link s�resinde �al��acak link free olay� olu�turuldu,sonraki switch de�eri olarak" << path[0] << "de�eri verildi ve global kuyru�a eklendi" << endl;
									Olay link_free(linkSuresi + simulation_time, LINK_FREE, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
									link_free.portID = path[2];
									//link_free.sonrakiPortID = path[1];
									//link_free.sonrakiswitchId = path[0];
									sorted_queue.push(link_free);
								}
								Olay routing(simulation_time, ROUTING, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
								cout << bulunulanSwitchId << " No lu switch i�in Yeni bir routing olay� olu�turuldu ve global kuyru�a eklendi" << endl;
								sorted_queue.push(routing);
							}
						}
					}
					else {
						cout << "hedef compute ba�l� olan swtiche gelindi" << endl;
						int targetPortIndex = 0;
						cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunua mesaj ta��nacak" << endl;
						LimitedQueue& recvQueue = switches[event.getBulunulanSwitchId()].ports[portIndex].HW_fifo_recv;
						LimitedQueue& sendQueue = switches[event.getBulunulanSwitchId()].ports[0].HW_fifo_send;

						if (sendQueue.size() < sendQueue.full) {
							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunda yer var m� diye kontrol edildi." << endl;
							cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portundan mesaj ��kar�ld�" << endl;
							recvQueue.pop();
							switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit = switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit + 1;
							//event.olayReceived = event.olayReceived + 1;
							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portuna ��kan mesaj eklendi." << endl;
							sendQueue.push(event);

							if (switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit > 0 && switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree == true) {
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FREE si ve credisi yeterlimi diye bak�ld�. Credisi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;
								sendQueue.pop();
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portundan mesaj ��kar�ld�" << endl;
								switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree = false;
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FALSE yap�ld�" << endl;
								switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit - 1;
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun credisi 1 d���r�ld�, G�ncel Credi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;

								event.setOlayZamani(linkSuresi + simulation_time);
								cout << "Olay�n t�r� message yap�ld�, zaman� + link s�resi yap�ld�," << "  �ncekiswitch no suda" << bulunulanSwitchId << " olarak belirlendi, bu mesaj hedef compte e gidecek ve global kuyru�a eklendi " << endl;
								//receivde  =0 kodunu ekle								
								event.setOlayTuru(MESSAGE);

								event.hedefKontrol = bulunulanSwitchId;

								sorted_queue.push(event);
								cout << "+ link s�resinde �al��acak link free olay� olu�turuldu,sonraki switch de�eri olarak" << bulunulanSwitchId << "de�eri verildi ve global kuyru�a eklendi" << endl;
								Olay link_free(linkSuresi + simulation_time, LINK_FREE, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
								//link_free.sonrakiswitchId = bulunulanSwitchId;
								sorted_queue.push(link_free);
							}
							Olay routing(simulation_time, ROUTING, baslang�cNodeId, hedefComputeId, bulunulanSwitchId);
							cout << bulunulanSwitchId << " No lu switch i�in Yeni bir routing olay� olu�turuldu ve global kuyru�a eklendi" << endl;
							sorted_queue.push(routing);

						}
					}
				}
			}
		}
		break;
		case TRY_AGAIN:
			cout << baslang�cNodeId << " Nolu Compute i�in �uan Link Dolu oldu�u i�in ba�ka i�lem yap�lmad�" << endl;
		default:
			cout << "Ge�ersiz olay t�r� giri�i!" << endl;
		}






		sorted_queue.pop(); // �lk olay� kuyruktan ��kar

	
	return true; // D�ng�ye devam
}







int main()
{
	locale::global(locale(""));

	vector<Switch> switches;
	vector<Compute> computes;






	//cout << "Sim�lasyon 12 adet switch ve bu switchlere ba�l� 12 adet compute olacak �ekilde olu�turuldu" << endl;
	//cout << "Devam etmek i�in Enter'a bas�n�z.";
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');

	//cout << "---------------------------------------------------------------------------" << endl;

	// 12 adet switch ve compute olu�turma
	for (int i = 0; i < 12; ++i) {
		Switch mySwitch(i, "Switch " + to_string(i));
		switches.push_back(mySwitch);
		cout << i << " nolu switch olu�turuldu." << endl;

		Compute compute(i, "Compute " + to_string(i));
		computes.push_back(compute);
		cout << i << " nolu compute olu�turuldu." << endl;

		


	}

	//Her switchi bir compute ile ba�lama
	for (int i = 0; i < 12; ++i) {
		computes[i].port_compute.kime = &switches[i].ports[0]; // Her compute cihaz�n� ilgili switch'in portuna ba�lama
		switches[i].ports[0].kime = &computes[i].port_compute;


		cout << "Switch " << i << " ile Compute " << i << " aras�ndaki ba�lant�:" << endl;
		cout << "Switch " << i << "'in Compute Port'u -> Compute " << i << " �n compute portuna ba�l�" << endl;
		cout << "Compute " << i << "'in Compute Port'u -> Switch " << i << " �n compute portuna ba�l�" << endl;
		cout << "---------------------------------------------------------------------------" << endl;
	}

	for (int m = 0; m < 12; ++m) {

		switches[m].ports[1].kime = &switches[(m-4+12)%12].ports[3];
		switches[m].ports[2].kime = &switches[(m+1)%4+4*(m/4)].ports[4];
		switches[m].ports[3].kime = &switches[(m+4)%12].ports[1];
		switches[m].ports[4].kime = &switches[(m-1+4)%4+4*(m/4)].ports[2];

		cout << m << "Kuzey=>" << (m - 4 + 12) % 12 << " /  Do�u =>> " << (m + 1) % 4 + 4 * (m / 4) << " /  g�ney =>" << (m + 4) % 12<< "Bat� =>" << (m - 1 + 4) % 4 + 4 * (m / 4) << endl;
		cout << "Switch " << m << "'Kuzeyindeki port " << (m - 4 + 12) % 12 << " Nolu switchin g�neyindeki porta ba�l�" << endl;
		cout << "Switch " << m << "'Do�usundaki port " << (m + 1) % 4 + 4 * (m / 4) << " Nolu switchin bat�s�ndaki porta ba�l�" << endl;
		cout << "Switch " << m << "'G�neyindeki port " << (m + 4) % 12 << " Nolu switchin kuzeyindeki porta ba�l�" << endl;
		cout << "Switch " << m << "'Bat�s�ndaki port " << (m - 1 + 4) % 4 + 4 * (m / 4) << " Nolu switchin do�usundaki porta ba�l�" << endl;

	}
	



	// ��lemlerin oldu�u s�ralanm�� olay kuyru�unun olu�umu
	priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>> sorted_event_queue(
		[](const Olay& o1, const Olay& o2) {
			return o1.getOlayZamani() > o2.getOlayZamani();
		}
	);
	cout << "G�ncel Simulate Time: " << simulation_time << endl;


	// Saat i�levini kullanarak rastgele tohum olu�tur
	srand(static_cast<unsigned int>(time(nullptr)));

	// Rastgele say� �retmek i�in random cihaz� olu�tur
	random_device rd;
	mt19937 gen(rd());

	// �retilecek say� aral���n� belirle
	int minRange = 0;
	int maxRange = 11;
	
	
	
	// Test Opsionlar� 
	//------------------

	//1. Random say�lar alarak t�m sistemin �al��mas�:

	//for (int i = 0; i < 10; i++) {
	//	uniform_int_distribution<int> dis(minRange, maxRange);
	//	int x = dis(gen);
	//	int y = dis(gen);
	//	Olay ilkOlay(simulation_time + 100, NEW_MESSAGE, x, y, x); // �lk message �  olu�turma

	//	sorted_event_queue.push(ilkOlay);  	// Kuyru�a ilk olay� ekleme

	//}

	int x = 1;
	int y = 5;
	Olay ilkOlay(simulation_time + 100, NEW_MESSAGE, x, y, x); // �lk message �  olu�turma

	sorted_event_queue.push(ilkOlay);  	// Kuyru�a ilk olay� ekleme


	//2. Kullan�c�dan al�ndan bilgilere g�re sistemin tekli �al��mas�:
	
	//int x, y, z;
	//cout << "Datan�n ��kaca�� compute numaras�n� giriniz=>";
	//cin >> x;
	//cout << "Datan�n ula�aca�� compute numaras�n� giriniz=>";
	//cin >> y;
	//cout << "Data g�ndermek istedi�iniz simulasyon zaman�n� giriniz=>";
	//cin >> z;
	//Olay ilkOlay(simulation_time + z, "newmessage", x, y, x); // �lk message �  olu�turma
	////sorted_event_queue.push(ilkOlay);  	// Kuyru�a ilk olay� ekleme
	


	//3. Elle belirli say�da paket olu�turma

	//Olay ilkOlay3(simulation_time + 100, "newmessage", 0, 5,0 ); // �lk message �  olu�turma
	//sorted_event_queue.push(ilkOlay3);  	// Kuyru�a ilk olay� ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay4(simulation_time + 103, "newmessage", 0, 5, 0); // �lk message �  olu�turma
	//sorted_event_queue.push(ilkOlay4);  	// Kuyru�a ilk olay� ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay5(simulation_time + 104, "newmessage", 0, 5, 0); // �lk message �  olu�turma
	//sorted_event_queue.push(ilkOlay5);  	// Kuyru�a ilk olay� ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay6(simulation_time + 105, "newmessage", 0, 5, 0); // �lk message �  olu�turma
	//sorted_event_queue.push(ilkOlay6);  	// Kuyru�a ilk olay� ekleme
	//Olay ilkOlay7(simulation_time + 106, "newmessage", 0, 5, 0); // �lk message �  olu�turma
	////sorted_event_queue.push(ilkOlay7);  	// Kuyru�a ilk olay� ekleme



	// Kuyruktan bir olay kullanma

	bool continueRunning = true;

	while (continueRunning) {
		//cout << "Global Kuyruk:" << endl;
		//cout << "--------------" << endl;
		kuyrukGoster(sorted_event_queue);
		continueRunning = kuyrukOlayKullan(sorted_event_queue, switches, computes);

		cout << "----------------------------------------------------------------------------------------" << endl;

	}


	return 0;
}