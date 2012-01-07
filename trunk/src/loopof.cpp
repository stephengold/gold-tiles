#include "loopof.hpp"

LoopIterator::LoopIterator(unsigned max) {
	this->current = 0;
	this->max = max;
}

void LoopIterator::bump(void) {
	unsigned next = this->current + 1;
	if (next > this->max) {
		next = 1;
	}
	this->current = next;
}

unsigned LoopIterator::getIndex(void) {
	unsigned result = this->current;
	if (result >= this->max) {
		result = 0;
	}
	return result;
}

bool LoopIterator::isWrap(void) {
     return (this->current == this->max);
}

