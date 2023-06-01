local OrbitalRotor =
{
    lastMousePos = nil,
    xRotation = 0
}

function OrbitalRotor:OnAwake()
end

function OrbitalRotor:OnStart()
end

function OrbitalRotor:OnUpdate(deltaTime)
    if Inputs.GetMouseButton(MouseButton.BUTTON_RIGHT) then
        Inputs.LockMouse()
        mousePos = Inputs.GetMousePos()
        if self.lastMousePos ~= nil then
            self.xRotation = self.xRotation - (mousePos.x - self.lastMousePos.x) * 0.2
            self.owner:GetTransform():SetRotation(Quaternion.new(Vector3.new(0, self.xRotation, 0)))
        end
        self.lastMousePos = mousePos
    else
        Inputs.UnlockMouse()
        self.lastMousePos = nil
    end
end

function OrbitalRotor:OnEnd()
end

return OrbitalRotor
