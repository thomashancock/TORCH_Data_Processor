#ifndef EVENTTREEMANAGER_H
#define EVENTTREEMANAGER_H

// LOCAL
#include "RootManager.hpp"
#include "Event.hpp"

class EventTreeManager : public RootManager<Event> {
public:
	EventTreeManager(
		const std::string outfileName
	);

	void add(
		std::unique_ptr<Event> event
	) final;

private:
	void setUpBranches() { };

};

#endif /* EVENTTREEMANAGER_H */
