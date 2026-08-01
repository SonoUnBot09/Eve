#include <Eve/components/Camera.hpp>
#include <Eve/Entities/SystemDispatcher.hpp>
#include <Eve/Debug.hpp>
#include <Eve/Entities/EntityManager.hpp>
#include <Eve/components/Transform.hpp>

using namespace Eve::Entities;

void Start()
{
    
}

void Update(const float deltaTime)
{
    //Debug::print(std::to_string(deltaTime));
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);