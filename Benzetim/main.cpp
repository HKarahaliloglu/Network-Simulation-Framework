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



// Ýki boyutlu torus üzerinde sonraki elemaný, mesajýn çýktýðý portu ve gireceði portu bulma
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
	
// Kuyruktaki olaylarý ekrana yazdýran fonksiyon
void kuyrukGoster(const priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>>& sorted_queue) {
	priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>> copy_queue = sorted_queue;

	while (!copy_queue.empty()) {
		const Olay& front_element = copy_queue.top();
		EVENT_TYPE olayTuru = front_element.getOlayTuru();
		int time = front_element.getOlayZamani();
		int a = front_element.getBulunulanSwitchId();

		//cout << "------------" << endl;
		cout << "=> Olay Zamani: " << time << "  /  " << "Olay Türü: " << olayTuru << "  /  " << "Bulunnulan Switch ID  :" << a << endl;
		copy_queue.pop();
	}
}

// Kuyrukta bekleyen bir olayý kullanan fonksiyon
bool kuyrukOlayKullan(priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>>& sorted_queue, vector<Switch>& switches, vector<Compute>& computes) {
	if (sorted_queue.empty()) {
		cout << "Kuyrukta Bekleyen Olay Bulunmuyor" << endl;
		return false; // Döngüyü durdur
	}

		Olay ilkOlay = sorted_queue.top(); // Ýlk olaya eriþim
		EVENT_TYPE olayTuru = ilkOlay.getOlayTuru();
		int olayZamani = ilkOlay.getOlayZamani();
		int baslangýcNodeId = ilkOlay.getBaslangýcComputeId();
		int hedefComputeId = ilkOlay.getHedefComputeId();
		int bulunulanSwitchId = ilkOlay.getBulunulanSwitchId();
		Compute& baslangýcCompute = computes[baslangýcNodeId];  // ilkCompute nesnesine eriþebilirsiniz
		Compute& hedefCompute = computes[hedefComputeId];  // HedefCompute nesnesine eriþebilirsiniz
		Switch& bulunulanNode = switches[bulunulanSwitchId];  // BulunulanSwitch nesnesine eriþebilirsiniz

		simulation_time = olayZamani;
		cout << "Kuruktan çýkan :" << simulation_time << olayTuru << endl;
		cout << "Güncel Sümulation Zamaný ==>" << olayZamani << endl;


		switch (olayTuru) {
		case MESSAGE:
		{

			cout << olayZamani << " olay zamanlý message olayý çalýþtý" << endl;

			if (baslangýcCompute.id == bulunulanSwitchId) {

				bulunulanNode.ports[0].HW_fifo_recv.push(ilkOlay);
				cout << bulunulanSwitchId << " Numaralý switchin compute portunun HW_fifo_ recv ine olay eklendi" << endl;
				// Kuyruktaki eleman sayýsýný bulma
				int itemCount = bulunulanNode.ports[0].HW_fifo_recv.size();
				//cout << " baþlangýçdaki ilk switchin recv Kuyruktaki eleman --sayisi: " << itemCount << std::endl;
			}

			else if (hedefComputeId == ilkOlay.hedefKontrol) {
				hedefCompute.port_compute.HW_fifo_recv.push(ilkOlay);
				cout << hedefComputeId << " Numaralý compute ün compute portunun HW_fifo_ recv ine mesaj eklendi ve mesaj alýnýp tüketildi." << endl;
				hedefCompute.port_compute.HW_fifo_recv.pop();
			}

			else if (bulunulanNode.type == 1) {


				bulunulanNode.ports[ilkOlay.sonrakiPortID].HW_fifo_recv.push(ilkOlay);
				cout << bulunulanNode.id << "Numaralý switchin " << ilkOlay.sonrakiPortID << "No lu portuna mesaj eklendi" << endl;
			}
			ilkOlay.setOlayTuru(ROUTING);
			//Olay routing(simulation_time, "routing", baslangýcNodeId, hedefComputeId, bulunulanSwitchId);		
			cout << bulunulanSwitchId << " No lu switch için Routing olayý oluþturuldu ve global kuyruða eklendi" << endl;
			sorted_queue.push(ilkOlay);
		}

		break;
		
		case NEW_MESSAGE:
		{
			cout << "newmessage olayý çalýþtý" << endl;

			if (baslangýcCompute.port_compute.HW_fifo_send.size() == baslangýcCompute.port_compute.HW_fifo_send.full) {
				//Olay tryAgain(simulation_time, "tryagain", baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
				//sorted_queue.push(tryAgain);
				ilkOlay.setOlayZamani(olayZamani + 1);
				sorted_queue.push(ilkOlay);
				cout << baslangýcNodeId << " No lu Compute ün send Fifosu dolu olduðu için newmessage olayý çalýþmadý, new message çalýþacaðý süre 1 artýrýlýp tekrar globl kuyruða eklendi." << endl;

			}
			else {

				baslangýcCompute.port_compute.HW_fifo_send.push(ilkOlay);
				int itemCount = baslangýcCompute.port_compute.HW_fifo_send.size();
				cout << baslangýcNodeId << " Nolu compute node un HW_fifo_send ine yeni olay eklendi! " << endl;
				cout << baslangýcNodeId << " No lu computenin send fifosundaki eleman sayisi: " << itemCount << endl;

				if (baslangýcCompute.port_compute.linkfree == true && baslangýcCompute.port_compute.credit > 0) {
					cout << baslangýcNodeId << " numaralý compute node un linki free ve credisi kontrol edildi" << endl;
					cout << baslangýcNodeId << " Nolu compute node un HW_fifo_send inde bekleyen ilk olay çýkarýldý " << endl;
					baslangýcCompute.port_compute.HW_fifo_send.pop();
					baslangýcCompute.port_compute.credit = baslangýcCompute.port_compute.credit - 1;
					cout << baslangýcNodeId << " numaralý compute node un credisi 1 azaltýldý , yeni credi deðeri => " << baslangýcCompute.port_compute.credit << endl;
					baslangýcCompute.port_compute.linkfree = false;
					cout << baslangýcNodeId << " numaralý compute node un linki false yapýldý" << endl;
					//cout << ilkOlay.olayReceived << endl;
					//cout << baslangýcCompute.port_compute.port_Received << endl;
					//ilkOlay.olayReceived = baslangýcCompute.port_compute.port_Received;
					//baslangýcCompute.port_compute.port_Received = 0;
					ilkOlay.setOlayTuru(MESSAGE);
					ilkOlay.setOlayZamani(simulation_time + linkSuresi);
					cout << "Olayýn türü mesaja çevrildi , simulasyon zamaný +link yapýldý ve global kuyruða eklendi" << endl;
					sorted_queue.push(ilkOlay);

					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
					//link_free.sonrakiswitchId = bulunulanSwitchId;
					link_free.oncekiswitchId = bulunulanSwitchId;
					//link_free.portID = NULL;
					cout << "linkfree olayý oluþturuldu, time ý + link süresi þeklinde ayarlandý ve global kuyruða eklendi" << endl;
					sorted_queue.push(link_free);
				}
			}
		}
		break;

		case LINK_FREE:
		{
			cout << bulunulanSwitchId << " No lu switch için link_free olayý çalýþtý" << endl;

			if (ilkOlay.oncekiswitchId == baslangýcCompute.id) {
				cout << "link free olayý " << baslangýcNodeId << " No lu compute için çalýþtý ve send linki tekrar TRUE hale getirildi" << endl;
				baslangýcCompute.port_compute.linkfree = true;

				if (!baslangýcCompute.port_compute.HW_fifo_send.empty() && baslangýcCompute.port_compute.credit > 0) {
					Olay& event = baslangýcCompute.port_compute.HW_fifo_send.front();
					cout << baslangýcNodeId << " No lu compute ün send inde bekleyen olay var ve kredisi 0 da büyük olduðu için , kredi miktarý =>> " << baslangýcCompute.port_compute.credit << endl;
					baslangýcCompute.port_compute.HW_fifo_send.pop();
					cout << baslangýcNodeId << " Nolu compute node un HW_fifo_send inde bekleyen ilk olay çýkarýldý " << endl;
					baslangýcCompute.port_compute.credit = baslangýcCompute.port_compute.credit - 1;
					cout << baslangýcNodeId << " numaralý compute node un credisi 1 azaltýldý , yeni credi deðeri => " << baslangýcCompute.port_compute.credit << endl;
					baslangýcCompute.port_compute.linkfree = false;
					cout << baslangýcNodeId << " numaralý compute node un linki false yapýldý" << endl;



					event.setOlayTuru(MESSAGE);
					event.setOlayZamani(simulation_time + linkSuresi);
					cout << "Olayýn türü mesaja çevrildi , simulasyon zamaný +link yapýldý ve global kuyruða eklendi" << endl;
					sorted_queue.push(event);
					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
					link_free.oncekiswitchId = baslangýcNodeId;
					//link_free.portID = NULL;
					sorted_queue.push(link_free);
					cout << "linkfree olayý oluþturuldu, time ý + link süresi þeklinde ayarlandý ve global kuyruða eklendi" << endl;

					//Olay routing(simulation_time, "routing", baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
					//sorted_queue.push(routing);
				}
			}
			else if (bulunulanSwitchId == hedefComputeId) {
				bulunulanNode.ports[0].linkfree = true;
				cout << "link free olayý " << hedefComputeId << " No lu switch için çalýþtý ve send linki tekrar TRUE hale getirildi" << endl;
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
					cout << "Sonraki switchin recv port numarasý: " << path[1] << endl;
					cout << "Bulunulan switchin send port numarasý: " << path[2] << endl;

					ilkOlay.setOlayTuru(MESSAGE);
					ilkOlay.setOlayZamani(simulation_time + linkSuresi);
					ilkOlay.setBulunulanSwitchId(path[0]);
					ilkOlay.sonrakiPortID = path[1];

					sorted_queue.push(ilkOlay);
					Olay link_free(simulation_time + linkSuresi, LINK_FREE, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
					link_free.portID = path[2];
					sorted_queue.push(link_free);
				}
			}
		}
		break;
		case ROUTING:
		{
			vector<int> portIndices = { 0, 1, 2, 3, 4 };
			cout << bulunulanSwitchId << " No lu switch için routing olayý çalýþtý" << endl;
			for (int portIndex : portIndices) {
				if (!switches[bulunulanSwitchId].ports[portIndex].HW_fifo_recv.empty()) {
					cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portunun recv inde bulunan olay için routing yapýlacak" << endl;
					Olay& event = switches[bulunulanSwitchId].ports[portIndex].HW_fifo_recv.front();


					int start = event.getBulunulanSwitchId();
					int target = event.getHedefComputeId();

					vector<int> path = findTorusPath(start, target);


					if (path.size() >= 1) {
						cout << "Sonraki switch ID si: " << path[0] << endl;
						cout << "Sonraki switchin recv port numarasý: " << path[1] << endl;
						cout << "Bulunulan switchin send port numarasý: " << path[2] << endl;
						int targetPortIndex = path[2];

						if (targetPortIndex != -1) {

							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunua mesaj taþýnacak" << endl;
							LimitedQueue& recvQueue = switches[event.getBulunulanSwitchId()].ports[portIndex].HW_fifo_recv;
							LimitedQueue& sendQueue = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].HW_fifo_send;

							if (sendQueue.size() < sendQueue.full) {
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunda yer var mý diye kontrol edildi." << endl;
								cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portundan mesaj çýkarýldý" << endl;
								recvQueue.pop();
								//cout << switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit << endl;
								switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit = switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit + 1; //--------------------------------------------------------------------------------------------
								//cout << switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit << endl;
								//event.olayReceived = event.olayReceived + 1; // naptýðýný tam anlamadým
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portuna çýkan mesaj eklendi." << endl;
								sendQueue.push(event);

								if (switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit > 0 && switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree == true) {

									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FREE mi ve credisi yeterlimi diye bakýldý. Credisi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;
									sendQueue.pop();
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portundan mesaj çýkarýldý" << endl;
									switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree = false;
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FALSE yapýldý" << endl;
									switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit - 1;
									cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun credisi 1 düþürüldü, Güncel Credi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;

									event.setOlayZamani(linkSuresi + simulation_time);
									//receivde  =0 kodunu ekle
									cout << "Olayýn türü message yapýldý, zamaný + link süresi yapýldý, bulunacaðý switch numarasý " << path[0] << " yapýldý. Öncekiswitch no suda" << bulunulanSwitchId << " olarak belirlendi ve global kuyruða eklendi " << endl;

									event.setBulunulanSwitchId(path[0]);
									event.setOlayTuru(MESSAGE);
									event.sonrakiPortID = path[1];

									sorted_queue.push(event);
									cout << "+ link süresinde çalýþacak link free olayý oluþturuldu,sonraki switch deðeri olarak" << path[0] << "deðeri verildi ve global kuyruða eklendi" << endl;
									Olay link_free(linkSuresi + simulation_time, LINK_FREE, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
									link_free.portID = path[2];
									//link_free.sonrakiPortID = path[1];
									//link_free.sonrakiswitchId = path[0];
									sorted_queue.push(link_free);
								}
								Olay routing(simulation_time, ROUTING, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
								cout << bulunulanSwitchId << " No lu switch için Yeni bir routing olayý oluþturuldu ve global kuyruða eklendi" << endl;
								sorted_queue.push(routing);
							}
						}
					}
					else {
						cout << "hedef compute baðlý olan swtiche gelindi" << endl;
						int targetPortIndex = 0;
						cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunua mesaj taþýnacak" << endl;
						LimitedQueue& recvQueue = switches[event.getBulunulanSwitchId()].ports[portIndex].HW_fifo_recv;
						LimitedQueue& sendQueue = switches[event.getBulunulanSwitchId()].ports[0].HW_fifo_send;

						if (sendQueue.size() < sendQueue.full) {
							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunda yer var mý diye kontrol edildi." << endl;
							cout << bulunulanSwitchId << " No lu switchin " << portIndex << " No lu portundan mesaj çýkarýldý" << endl;
							recvQueue.pop();
							switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit = switches[event.getBulunulanSwitchId()].ports[portIndex].kime->credit + 1;
							//event.olayReceived = event.olayReceived + 1;
							cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portuna çýkan mesaj eklendi." << endl;
							sendQueue.push(event);

							if (switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit > 0 && switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree == true) {
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FREE si ve credisi yeterlimi diye bakýldý. Credisi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;
								sendQueue.pop();
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portundan mesaj çýkarýldý" << endl;
								switches[event.getBulunulanSwitchId()].ports[targetPortIndex].linkfree = false;
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun linki FALSE yapýldý" << endl;
								switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit = switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit - 1;
								cout << bulunulanSwitchId << " No lu switchin " << targetPortIndex << " No lu portunun credisi 1 düþürüldü, Güncel Credi =>>" << switches[event.getBulunulanSwitchId()].ports[targetPortIndex].credit << endl;

								event.setOlayZamani(linkSuresi + simulation_time);
								cout << "Olayýn türü message yapýldý, zamaný + link süresi yapýldý," << "  Öncekiswitch no suda" << bulunulanSwitchId << " olarak belirlendi, bu mesaj hedef compte e gidecek ve global kuyruða eklendi " << endl;
								//receivde  =0 kodunu ekle								
								event.setOlayTuru(MESSAGE);

								event.hedefKontrol = bulunulanSwitchId;

								sorted_queue.push(event);
								cout << "+ link süresinde çalýþacak link free olayý oluþturuldu,sonraki switch deðeri olarak" << bulunulanSwitchId << "deðeri verildi ve global kuyruða eklendi" << endl;
								Olay link_free(linkSuresi + simulation_time, LINK_FREE, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
								//link_free.sonrakiswitchId = bulunulanSwitchId;
								sorted_queue.push(link_free);
							}
							Olay routing(simulation_time, ROUTING, baslangýcNodeId, hedefComputeId, bulunulanSwitchId);
							cout << bulunulanSwitchId << " No lu switch için Yeni bir routing olayý oluþturuldu ve global kuyruða eklendi" << endl;
							sorted_queue.push(routing);

						}
					}
				}
			}
		}
		break;
		case TRY_AGAIN:
			cout << baslangýcNodeId << " Nolu Compute için Þuan Link Dolu olduðu için baþka iþlem yapýlmadý" << endl;
		default:
			cout << "Geçersiz olay türü giriþi!" << endl;
		}






		sorted_queue.pop(); // Ýlk olayý kuyruktan çýkar

	
	return true; // Döngüye devam
}







int main()
{
	locale::global(locale(""));

	vector<Switch> switches;
	vector<Compute> computes;






	//cout << "Simülasyon 12 adet switch ve bu switchlere baðlý 12 adet compute olacak þekilde oluþturuldu" << endl;
	//cout << "Devam etmek için Enter'a basýnýz.";
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');

	//cout << "---------------------------------------------------------------------------" << endl;

	// 12 adet switch ve compute oluþturma
	for (int i = 0; i < 12; ++i) {
		Switch mySwitch(i, "Switch " + to_string(i));
		switches.push_back(mySwitch);
		cout << i << " nolu switch oluþturuldu." << endl;

		Compute compute(i, "Compute " + to_string(i));
		computes.push_back(compute);
		cout << i << " nolu compute oluþturuldu." << endl;

		


	}

	//Her switchi bir compute ile baðlama
	for (int i = 0; i < 12; ++i) {
		computes[i].port_compute.kime = &switches[i].ports[0]; // Her compute cihazýný ilgili switch'in portuna baðlama
		switches[i].ports[0].kime = &computes[i].port_compute;


		cout << "Switch " << i << " ile Compute " << i << " arasýndaki baðlantý:" << endl;
		cout << "Switch " << i << "'in Compute Port'u -> Compute " << i << " ýn compute portuna baðlý" << endl;
		cout << "Compute " << i << "'in Compute Port'u -> Switch " << i << " ýn compute portuna baðlý" << endl;
		cout << "---------------------------------------------------------------------------" << endl;
	}

	for (int m = 0; m < 12; ++m) {

		switches[m].ports[1].kime = &switches[(m-4+12)%12].ports[3];
		switches[m].ports[2].kime = &switches[(m+1)%4+4*(m/4)].ports[4];
		switches[m].ports[3].kime = &switches[(m+4)%12].ports[1];
		switches[m].ports[4].kime = &switches[(m-1+4)%4+4*(m/4)].ports[2];

		cout << m << "Kuzey=>" << (m - 4 + 12) % 12 << " /  Doðu =>> " << (m + 1) % 4 + 4 * (m / 4) << " /  güney =>" << (m + 4) % 12<< "Batý =>" << (m - 1 + 4) % 4 + 4 * (m / 4) << endl;
		cout << "Switch " << m << "'Kuzeyindeki port " << (m - 4 + 12) % 12 << " Nolu switchin güneyindeki porta baðlý" << endl;
		cout << "Switch " << m << "'Doðusundaki port " << (m + 1) % 4 + 4 * (m / 4) << " Nolu switchin batýsýndaki porta baðlý" << endl;
		cout << "Switch " << m << "'Güneyindeki port " << (m + 4) % 12 << " Nolu switchin kuzeyindeki porta baðlý" << endl;
		cout << "Switch " << m << "'Batýsýndaki port " << (m - 1 + 4) % 4 + 4 * (m / 4) << " Nolu switchin doðusundaki porta baðlý" << endl;

	}
	



	// Ýþlemlerin olduðu sýralanmýþ olay kuyruðunun oluþumu
	priority_queue<Olay, vector<Olay>, function<bool(const Olay&, const Olay&)>> sorted_event_queue(
		[](const Olay& o1, const Olay& o2) {
			return o1.getOlayZamani() > o2.getOlayZamani();
		}
	);
	cout << "Güncel Simulate Time: " << simulation_time << endl;


	// Saat iþlevini kullanarak rastgele tohum oluþtur
	srand(static_cast<unsigned int>(time(nullptr)));

	// Rastgele sayý üretmek için random cihazý oluþtur
	random_device rd;
	mt19937 gen(rd());

	// Üretilecek sayý aralýðýný belirle
	int minRange = 0;
	int maxRange = 11;
	
	
	
	// Test Opsionlarý 
	//------------------

	//1. Random sayýlar alarak tüm sistemin çalýþmasý:

	//for (int i = 0; i < 10; i++) {
	//	uniform_int_distribution<int> dis(minRange, maxRange);
	//	int x = dis(gen);
	//	int y = dis(gen);
	//	Olay ilkOlay(simulation_time + 100, NEW_MESSAGE, x, y, x); // Ýlk message ý  oluþturma

	//	sorted_event_queue.push(ilkOlay);  	// Kuyruða ilk olayý ekleme

	//}

	int x = 1;
	int y = 5;
	Olay ilkOlay(simulation_time + 100, NEW_MESSAGE, x, y, x); // Ýlk message ý  oluþturma

	sorted_event_queue.push(ilkOlay);  	// Kuyruða ilk olayý ekleme


	//2. Kullanýcýdan alýndan bilgilere göre sistemin tekli çalýþmasý:
	
	//int x, y, z;
	//cout << "Datanýn çýkacaðý compute numarasýný giriniz=>";
	//cin >> x;
	//cout << "Datanýn ulaþacaðý compute numarasýný giriniz=>";
	//cin >> y;
	//cout << "Data göndermek istediðiniz simulasyon zamanýný giriniz=>";
	//cin >> z;
	//Olay ilkOlay(simulation_time + z, "newmessage", x, y, x); // Ýlk message ý  oluþturma
	////sorted_event_queue.push(ilkOlay);  	// Kuyruða ilk olayý ekleme
	


	//3. Elle belirli sayýda paket oluþturma

	//Olay ilkOlay3(simulation_time + 100, "newmessage", 0, 5,0 ); // Ýlk message ý  oluþturma
	//sorted_event_queue.push(ilkOlay3);  	// Kuyruða ilk olayý ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay4(simulation_time + 103, "newmessage", 0, 5, 0); // Ýlk message ý  oluþturma
	//sorted_event_queue.push(ilkOlay4);  	// Kuyruða ilk olayý ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay5(simulation_time + 104, "newmessage", 0, 5, 0); // Ýlk message ý  oluþturma
	//sorted_event_queue.push(ilkOlay5);  	// Kuyruða ilk olayý ekleme
	//cout << "---------------------------------------------------------------------------" << endl;
	//Olay ilkOlay6(simulation_time + 105, "newmessage", 0, 5, 0); // Ýlk message ý  oluþturma
	//sorted_event_queue.push(ilkOlay6);  	// Kuyruða ilk olayý ekleme
	//Olay ilkOlay7(simulation_time + 106, "newmessage", 0, 5, 0); // Ýlk message ý  oluþturma
	////sorted_event_queue.push(ilkOlay7);  	// Kuyruða ilk olayý ekleme



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