#pragma once

#include "Inivar.hpp"

class CollisionBox {
public: 
	CollisionBox();
	CollisionBox(const glm::vec2& p, const glm::vec4& s, const bool i)
		: pos(p), size(s), is_trigger(i) {}
	~CollisionBox();

	inline void SetPos(const glm::vec2& p) { pos = p; }
	inline void SetSize(const glm::vec4& s) { size = s; }
	inline void SetTrigger(const bool i) { is_trigger = i; }
	inline glm::vec2 GetPos() const { return pos; }
	inline glm::vec4 GetSize() const { return size; }
	inline bool IsTrigger() const { return is_trigger; }

	virtual void OnCollisionEnter() {}
	virtual void OnTriggerEnter() {}

private:
	glm::vec2 pos {0};
	glm::vec4 size {0};
	bool is_trigger = false;
};