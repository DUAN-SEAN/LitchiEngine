local SponzaExplorer =
{
    skip = false,
    transform =  nil,
    cameraFade = nil,

    destPositions =
    { 
        Vector3.new(6, 2.2, -0.2),
        Vector3.new(-11.05, 2.2, -0.2),
        Vector3.new(-12.05, 2.0, 3.2),
        Vector3.new(10, 2.0, 3.950),
        Vector3.new(9.5, 1.25, -0.2),
        Vector3.new(-10, 10.5, -0.2),
        Vector3.new(-6.5, 6.1, -3.9),
        Vector3.new(11, 6.1, -5),
        Vector3.new(11, 4.5, -0.2)
    },

    destRotations =
    {
        Quaternion.new(Vector3.new(0, 90, 0)),
        Quaternion.new(Vector3.new(0, 0, 0)),
        Quaternion.new(Vector3.new(30, 0, 0)),
        Quaternion.new(Vector3.new(0, -180, 0)),
        Quaternion.new(Vector3.new(-30, -90, 0)),
        Quaternion.new(Vector3.new(20, 90, 0)),
        Quaternion.new(Vector3.new(0, 0, 0)),
        Quaternion.new(Vector3.new(0, 0, 0)),
        Quaternion.new(Vector3.new(90, 0, 0))
    },

    destSpeeds =
    {
        0.002,
        0.002,
        0.004,
        0.0015,
        0.003,
        0.0015,
        0.005,
        0.008,
        0.004
    },

    destionationIndex = 1,
    alpha = 0,
    timer = 0
}

function SponzaExplorer:OnAwake()
    self.transform = self.owner:GetTransform()
    self.cameraFade = Scenes.GetCurrentScene():FindActorByName("CameraFade")
    self.cameraFade:GetMaterialRenderer():SetUserMatrixElement(0, 0, 0.0)
end

function SponzaExplorer:OnEnable()
    Inputs.LockMouse()
end

function SponzaExplorer:OnUpdate(deltaTime)
    destPosition    = self.destPositions[self.destionationIndex]
    destRotation    = self.destRotations[self.destionationIndex]
    destSpeed       = self.destSpeeds[self.destionationIndex]

    speedCoeff = 1.0
    if Inputs.GetKey(Key.F) then
        speedCoeff = speedCoeff * 100
    end

    if Inputs.GetKey(Key.R) then
        speedCoeff = speedCoeff * 0.01
    end

    if Inputs.GetKeyDown(Key.SPACE) then
        self.skip = true
    end

    if destPosition ~= nil and not self.skip then
        self.alpha = self.alpha + destSpeed * speedCoeff
        self.transform:SetLocalPosition(Vector3.Lerp(self.transform:GetLocalPosition(), destPosition, self.alpha * deltaTime))
        self.transform:SetLocalRotation(Quaternion.Slerp(self.transform:GetLocalRotation(), destRotation, self.alpha * deltaTime))
        if Vector3.Distance(self.transform:GetPosition(), destPosition) < 0.2 then
            self.destionationIndex = self.destionationIndex + 1
            self.alpha = 0
        end
    else
        self.cameraFade:SetActive(true)
        self.timer = self.timer + deltaTime
        self.cameraFade:GetMaterialRenderer():SetUserMatrixElement(0, 0, self.timer)

        if self.timer >= 1.0 then
            Scenes.GetCurrentScene():FindActorByName("Player"):SetActive(true)
            self.owner:SetActive(false)
        end
    end
end

return SponzaExplorer
