#include "CollisionManager.h"
#include "Circle.h"
#include "AABB.h"

bool CheckCollisionCircleCircle(Shape* circle1, glm::vec3 c1Pos, 
	Shape* circle2, glm::vec3 c2Pos, Contact* contact)
{
	float sqDistance;
	float circle1Radius, circle2Radius;
	float radiusSum;

	sqDistance = 
		(c1Pos.x - c2Pos.x) * (c1Pos.x - c2Pos.x) + 
		(c1Pos.y - c2Pos.y) * (c1Pos.y - c2Pos.y);

	circle1Radius = static_cast<Circle*>(circle1)->Radius;
	circle2Radius = static_cast<Circle*>(circle2)->Radius;

	radiusSum = circle1Radius + circle2Radius;

	if (sqDistance <= radiusSum*radiusSum)
	{
		contact->contactBodies[0] = circle1->OwnerBody;
		contact->contactBodies[1] = circle2->OwnerBody;
		return true;
	}

	return false;

}

bool CheckCollisionCircleAABB(Shape* circleShape, glm::vec3 cPos,
	Shape* aabbShape, glm::vec3 rectPos, Contact* contact)
{
	Circle* shapeCircle = (Circle*)circleShape;
	AABB* shapeAABB = (AABB*)aabbShape;

	float circleCenterX = cPos.x;
	float circleCenterY = cPos.y;


	float rectCenterX = rectPos.x;
	float rectCenterY = rectPos.y;

	float rectLeft = rectCenterX + shapeAABB->Left;
	float rectRigth = rectCenterX + shapeAABB->Right;
	float rectTop = rectCenterY + shapeAABB->Top;
	float rectBottom = rectCenterY + shapeAABB->Bottom;

	float recNearesPointToCircleX, recNearesPointToCircleY = 0;

	float distance = 0;

	if (circleCenterX < rectLeft)
	{
		recNearesPointToCircleX = rectLeft;
	}
	else if (circleCenterX > rectRigth)
	{
		recNearesPointToCircleX = rectRigth;
	}
	else
	{
		recNearesPointToCircleX = circleCenterX;
	}

	if (circleCenterY < rectBottom)
	{
		recNearesPointToCircleY = rectBottom;
	}
	else if (circleCenterY > rectTop)
	{
		recNearesPointToCircleY = rectTop;
	}
	else
	{
		recNearesPointToCircleY = circleCenterY;
	}

	float sqDistance = (recNearesPointToCircleX - circleCenterX) * (recNearesPointToCircleX - circleCenterX) +
		(recNearesPointToCircleY - circleCenterY) * (recNearesPointToCircleY - circleCenterY);
	//distance = sqrt(sqDistance);

	if (sqDistance <= shapeCircle->Radius * shapeCircle->Radius)
	{
		contact->contactBodies[0] = shapeCircle->OwnerBody;
		contact->contactBodies[1] = shapeAABB->OwnerBody;
		return true;
	}
	return false;
}

bool CheckCollisionAABBAABB(Shape* aabb1Shape, glm::vec3 aabb1Pos,
	Shape* aabb2Shape, glm::vec3 aabb2Pos, Contact* contact)
{
	AABB* aabb1 = (AABB*)aabb1Shape;
	AABB* aabb2 = (AABB*)aabb2Shape;

	float rect1CenterX = aabb1Pos.x;
	float rect1CenterY = aabb1Pos.y;

	float rect2CenterX = aabb2Pos.x;
	float rect2CenterY = aabb2Pos.y;

	float rect1Left = rect1CenterX + aabb1->Left;
	float rect1Rigth = rect1CenterX + aabb1->Right;
	float rect1Top = rect1CenterY + aabb1->Top;
	float rect1Bottom = rect1CenterY + aabb1->Bottom;


	float rect2Left = rect2CenterX + aabb2->Left;
	float rect2Rigth = rect2CenterX + aabb2->Right;
	float rect2Top = rect2CenterY + aabb2->Top;
	float rect2Bottom = rect2CenterY + aabb2->Bottom;

	if (rect1Left > rect2Rigth) return false;
	if (rect1Rigth < rect2Left) return false;
	if (rect1Top < rect2Bottom) return false;
	if (rect1Bottom > rect2Top) return false;

	contact->contactBodies[0] = aabb1->OwnerBody;
	contact->contactBodies[1] = aabb2->OwnerBody;
	return true;
}

bool CheckCollisionAABBCircle(Shape* aabb1Shape, glm::vec3 aabb1Pos,
	Shape* aabb2Shape, glm::vec3 aabb2Pos, Contact* contact)
{
	return CheckCollisionCircleAABB(aabb2Shape, aabb2Pos, aabb1Shape, aabb1Pos, contact);
}

bool CollisionManager::GenerateContacts(Shape* a, glm::vec3 aPos, 
	Shape* b, glm::vec3 bPos, Contact* contact)
{
	return  (*CollsionTestTable[a->Type][b->Type])(a, aPos, b, bPos, contact);
}

Contact::Contact()
{
	contactBodies[0] = NULL;
	contactBodies[1] = NULL;
}

//void CollisionManager::ResetContacts()
//{

	//for (auto contact : Contacts)
	//{
	//	delete contact;
	//}
	//Contacts.clear();

	/*for (size_t i = 0; i < Contacts.size(); i++)
	{
		Contact* contactToDelete = Contacts.at(i);
		delete contactToDelete;
	}*/
	
//}


CollisionManager::CollisionManager()
{
	CollsionTestTable[Shape::ShapeType::CIRCLE][Shape::ShapeType::CIRCLE] = CheckCollisionCircleCircle;
	CollsionTestTable[Shape::ShapeType::CIRCLE][Shape::ShapeType::RECTANGLE] = CheckCollisionCircleAABB;
	CollsionTestTable[Shape::ShapeType::RECTANGLE][Shape::ShapeType::CIRCLE] = CheckCollisionAABBCircle;
	CollsionTestTable[Shape::ShapeType::RECTANGLE][Shape::ShapeType::RECTANGLE] = CheckCollisionAABBAABB;
}


CollisionManager::~CollisionManager()
{
	//ResetContacts();
}

