#include "lav/Threads/Event.h"

// STL zaglavlja
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

// Ostala zaglavlja
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace Threads;

Event::Event() : m_fd { ::eventfd(0, EFD_SEMAPHORE) }
, m_ready { false }
, m_unsat { false }
, m_finished { false }
{
	if (m_fd == InvalidEvent)
		throw std::runtime_error(std::string {
     "Threads::Greska u pravljenju Event objekta."
    } + ::strerror(errno));
}

Event::Event(Event &&e) : m_fd { e.m_fd }
, m_ready { e.m_ready }
, m_unsat { e.m_unsat }
, m_finished { e.m_finished }
{
	e.m_fd = InvalidEvent;
	e.m_ready = false;
}

Event &Event::operator=(Event &&e) {
	if (this != &e) {
		m_fd = e.m_fd;
		m_ready = e.m_ready;
		e.m_fd = InvalidEvent;
		e.m_ready = false;
	}
	return *this;

}

Event::~Event() {
	if (m_fd != InvalidEvent)
			::close(m_fd);
}

void Event::Deleter(Event *shared) { delete shared; }

std::shared_ptr<Event> Event::Create() {
	return std::shared_ptr<Event> { new Event
 {}
 , &Event::Deleter
}
;
}

bool Event::Ready() const { return m_ready; }

void Event::Signal(Sigval value) {
	uint64_t buff = 1;
	
	// Postavljamo vrednost koja je signalizirana
	// i s obzirom na to da moze da se desi da jedna nit signalizira vise puta
	// bitno je da pamtimo koji su sve tipovi signala bili
	if (value == Outcome::Finished)
		m_finished = true;
	else
		m_unsat = true;

	// Saljemo signal tako sto upisujemo vrednost u filedescriptor 
	// tj. povecavamo brojac fd-a
	if (::write(m_fd, &buff, sizeof(uint64_t)) == -1)
		throw std::runtime_error {
    std::string { "Threads::Neuspelo slanje signala u klasi Event. --- " }
    +::strerror(errno)
  }
 ;
}

Event::Sigval Event::Value(bool block) {
	// Proveravamo da li mozemo procitati vrednost
	if (!m_ready && !block)
		throw std::runtime_error {
    std::string { "Threads::Event nije spreman a pokusalo se sa citanjem." }
  }
 ;

	// Citamo vrednost iz fd-a tj. smanjujemo njegov brojac
	uint64_t buff;
	if (::read(m_fd, &buff, sizeof(uint64_t)) == -1)
		throw std::runtime_error {
    std::string { "Threads::Neuspelo citanje vrednosti signala. --- " }
    +::strerror(errno)
  }
 ;


	// Postavimo ready na false i procitamo vrednost
	// koja nas zanima. Nema potrebe da se cuvaju sve vrednosti signala
	// jer postoji hijerarhija signala koja nas zanima
	// TODO: Ovo mozda ne moze ovako
	if (m_unsat)
		return Outcome::Unsat;
	else if (m_finished)
		return Outcome::Finished;

    return Outcome::Finished;
}

std::vector<size_t>
Event::WaitForEvents(std::vector<std::shared_ptr<Event> > &events,
                     std::chrono::milliseconds waitMs, bool blosk) {
	using poll_t = struct pollfd;
	std::vector<poll_t> pevents;
	pevents.reserve(events.size());

	// Pravimo niz eventova za poll sistemski poziv
	std::transform(events.cbegin(), events.cend(), back_inserter(pevents),
                [ = ](const std::shared_ptr<Event> & eptr) {
				poll_t p;
				p.events = POLLIN;
				p.fd = eptr->m_fd;
				p.revents = 0;
				return p;
  });



	std::vector<size_t> readyEvents;
	int iWaitMs = static_cast<int>(waitMs.count());

	iWaitMs = !iWaitMs ? -1 : iWaitMs;

	// Cekamo na eventove 
	// -1 ako zelimo da cekamo dok se ne desi neki event, 0 inace
	int eventsOccured = ::poll(pevents.data(), pevents.size(), iWaitMs);

	if (eventsOccured == -1)
		throw std::runtime_error {
    std::string { "Threads::Greska prilikom cekanja na eventove. --- " }
    +::strerror(errno)
  }
 ;

	// Pravimo vektor indeksa eventova koji su spremni
	if (eventsOccured) {
		readyEvents.reserve(eventsOccured);

		for (auto it = pevents.cbegin(); it != pevents.end(); ++it) {
			// Uzimamo indeks eventa
			size_t idx = it - pevents.cbegin();

			if (it->revents == POLLIN) {
				readyEvents.push_back(idx);
				events[idx]->m_ready = true;
			} else if (it->revents == POLLERR) {
				throw std::runtime_error {
      std::string { "Threads::Greska prilikom cekanja na event: " }
      +std::to_string(idx) + " --- " + ::strerror(errno)
    }
    ;
		
			}
		}
	}
	
	return readyEvents;
}
