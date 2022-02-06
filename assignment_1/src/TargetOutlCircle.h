#pragma once
#ifndef __TargetOutlCircle__
#define __TargetOutlCircle__

#include "DisplayObject.h"

class TargetOutlCircle final : public DisplayObject {
public:
	TargetOutlCircle();
	~TargetOutlCircle();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

private:
	void m_move();
	void m_checkBounds();
	void m_reset();
};


#endif /* defined (__TargetOutlCircle__) */