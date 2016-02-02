#include <backend/jobs/updatetransformsjob.h>
#include <backend/physicsmanager.h>
#include <backend/backendtypes/physicsentity.h>
#include <backend/backendtypes/physicsbodyinfobackendnode.h>
#include <backend/backendtypes/physicstransform.h>

#include  <backend/physics_entities/physicsabstractrigidbody.h>
namespace Physics {

UpdateTransformsJob::UpdateTransformsJob(PhysicsManager* manager):
    QAspectJob()

{
    m_manager=manager;

}
void UpdateTransformsJob::run(){

    recursive_step(m_manager->rootEntityId(),QMatrix4x4());
}

void UpdateTransformsJob::recursive_step(Qt3DCore::QNodeId node_id, QMatrix4x4 parent_matrix){
    if(node_id.isNull()) return;
    QMatrix4x4 current_world_transform=parent_matrix;
    PhysicsEntity* entity=static_cast<PhysicsEntity*>(m_manager->m_resources[node_id]);
    if(!entity->transform().isNull()){
        PhysicsTransform* transform=static_cast<PhysicsTransform*>(m_manager->m_resources[entity->transform()]);
        current_world_transform=current_world_transform*transform->transformMatrix();
    }
    if(m_manager->m_Id2RigidBodies.contains(node_id)){
        if(!entity->physicsBodyInfo().isNull()){
            PhysicsBodyInfoBackendNode* body_info=static_cast<PhysicsBodyInfoBackendNode*>(m_manager->m_resources[entity->physicsBodyInfo()]);
            PhysicsAbstractRigidBody* rigid_body=static_cast<PhysicsAbstractRigidBody*>(m_manager->m_Id2RigidBodies[node_id]);
            current_world_transform=rigid_body->worldTransformation();
            /*If the object is not statics (or kinematic) then update the position*/
            //if(rigid_body->mass()!=0){
            body_info->setLocalTransform(parent_matrix.inverted()*rigid_body->worldTransformation());
            body_info->notifyFrontEnd("updateTransform");
            //}
        }
    }
    Q_FOREACH(Qt3DCore::QNodeId id, entity->childrenIds()){
        recursive_step(id,current_world_transform);
    }
}

}
