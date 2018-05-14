#ifndef EVENTTREEMANAGER_H
#define EVENTTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Event.hpp"

class EventTreeManager : public RootManager<Event> {
public:
	EventTreeManager();

	void add(
		Event event
	) final;


private:

};

#endif /* EVENTTREEMANAGER_H */
