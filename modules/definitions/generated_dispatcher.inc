// Generated jump table

static void undo_func_Tag(uint8_t* payload, ECS::ComponentRegistry& reg) {
    auto* d = reinterpret_cast<UndoTagData*>(payload);
    reg.getPool<ECS::Component::Tag>().get(d->entityID) = d->oldState;
}

static void undo_func_Transform(uint8_t* payload, ECS::ComponentRegistry& reg) {
    auto* d = reinterpret_cast<UndoTransformData*>(payload);
    reg.getPool<ECS::Component::Transform>().get(d->entityID) = d->oldState;
}

static void(*g_undoDispatchTable[(size_t)CommandType::COUNT])(uint8_t*, ECS::ComponentRegistry&) = {
    undo_func_Tag,
    undo_func_Transform,
};
