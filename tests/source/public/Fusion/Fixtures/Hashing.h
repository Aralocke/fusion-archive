/**
 * Copyright 2015-2022 Daniel Weiner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include <array>
#include <functional>
#include <span>
#include <string_view>

namespace Fusion
{
class HashingTests : public ::testing::Test
{
public:
    void SetUp() override {}
    void TearDown() override {}

public:
    struct HashTest
    {
        std::string_view md5;
        std::string_view sha1;
        std::string_view sha256;
        std::string_view data;
    };

    struct HmacTest
    {
        std::string_view md5;
        std::string_view sha1;
        std::string_view sha256;
        std::string_view key;
        std::string_view data;
    };

public:
    std::span<const HashTest> Tests() const;

    std::span<const HmacTest> HmacTests() const;

public:
    void CheckHash(std::span<const uint8_t> input, std::string_view hash);

    template<typename Method>
    void RunHashTest(
        std::function<void(typename Method::Digest&, const HashTest&)> fn)
    {
        using Digest = typename Method::Digest;

        for (const HashTest& test : Tests())
        {
            Digest digest;

            Method method;
            method.Process(test.data);
            method.Finish(digest);

            fn(digest, test);
        }
    }

    template<typename Method>
    void RunHmacTest(
        std::function<void(typename Method::Digest&, const HmacTest&)> fn)
    {
        using Digest = typename Method::Digest;

        for (const HmacTest& test : HmacTests())
        {
            std::span<const uint8_t> key{
                reinterpret_cast<const uint8_t*>(
                    test.key.data()),
                test.key.size()
            };

            std::span<const uint8_t> data{
                reinterpret_cast<const uint8_t*>(
                    test.data.data()),
                test.data.size()
            };

            Digest digest;
            HMAC<Method>(key, data, digest);

            fn(digest, test);
        }
    }

private:
    static constexpr auto HashData = std::to_array<HashTest>({
        {
            "d41d8cd98f00b204e9800998ecf8427e",
            "da39a3ee5e6b4b0d3255bfef95601890afd80709",
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            ""
        },
        {
            "0cc175b9c0f1b6a831c399e269772661",
            "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
            "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb",
            "a"
        },
        {
            "c3fcd3d76192e4007dfb496cca67e13b",
            "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
            "71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73",
            "abcdefghijklmnopqrstuvwxyz"
        },
        {
            "db89bb5ceab87f9c0fcc2ab36c189c2c",
            "cd36b370758a259b34845084a6cc38473cb95e27",
            "2d8c2f6d978ca21712b5f6de36c9d31fa8e96a4fa5d8ff8b0188dfb9e7c171bb",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor "
            "in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum."
        },
        {
            "3b6b0ad0d1f50a2a0aaa8d7d4572b210",
            "211311d377de4e6f87367164147ee05fcbddb1e1",
            "dc5a9eb47299650d291a85fd8146cc09ef4108952fbafa9c449809cb9a5c5566",
            "Communication is not possible. The shuttle has no power. Using "
            "the gravitational pull of a star to slingshot back in time? We "
            "are going to Starbase Montgomery for Engineering consultations "
            "prompted by minor read-out anomalies. Probes have recorded unusual "
            "levels of geological activity in all five planetary systems. "
            "Assemble a team. Look at records of the Drema quadrant. Would these "
            "scans detect artificial transmissions as well as natural signals?"
        },
        {
            "7bace49ddaa8403aabe338d504a51c33",
            "e72dde2be8b92dced8b661227693065fb3ee21c6",
            "25a18bed33e6d91439cf5b7a93b7a6ff55ca82825e39cf390cdc4040a4ef83e9",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ac "
            "tincidunt vitae semper quis lectus. Ac tortor dignissim convallis "
            "aenean et. Pulvinar pellentesque habitant morbi tristique senectus. "
            "Non arcu risus quis varius quam quisque id diam vel. Turpis in eu mi "
            "bibendum. Orci dapibus ultrices in iaculis nunc sed. Nibh ipsum "
            "consequat nisl vel pretium lectus quam id. Mi eget mauris pharetra et "
            "ultrices neque ornare aenean euismod. Iaculis urna id volutpat lacus "
            "laoreet non. Malesuada bibendum arcu vitae elementum. Sed egestas "
            "egestas fringilla phasellus faucibus scelerisque eleifend.Maecenas "
            "volutpat blandit aliquam etiam erat velit.Commodo sed egestas egestas "
            "fringilla phasellus faucibus.Rutrum tellus pellentesque eu tincidunt.Ac "
            "tortor dignissim convallis aenean et tortor at risus.Tincidunt augue "
            "interdum velit euismod in.Habitant morbi tristique senectus et netus "
            "et malesuada fames ac.Blandit turpis cursus in hac habitasse platea "
            "dictumst quisque.Et malesuada fames ac turpis.Vitae elementum curabitur "
            "vitae nunc sed velit dignissim.Eu scelerisque felis imperdiet proin.Neque "
            "volutpat ac tincidunt vitae."
        },
        {
            "f660262c0a88369433049667fc718269",
            "08b5fa5f56b658f25b027105c3097efa1beddeea",
            "30bc66680ff381626a965b6c6b024137320521274e0d5a5a5b8454aa6f32cc66",
            "I have reset the sensors to scan for frequencies outside the usual range. "
            "By emitting harmonic vibrations to shatter the lattices. We will monitor "
            "and adjust the frequency of the resonators. He has this ability of instantly "
            "interpreting and extrapolating any verbal communication he hears. It may be "
            "due to the envelope over the structure, causing hydrogen-carbon helix patterns "
            "throughout. I'm comparing the molecular integrity of that bubble against our phasers.\n"
            "\n"
            "It indicates a synchronic distortion in the areas emanating triolic waves.The "
            "cerebellum, the cerebral cortex, the brain stem,  the entire nervous system has been "
            "depleted of electrochemical energy.Any device like that would produce high levels of "
            "triolic waves.These walls have undergone some kind of selective molecular polarization.I "
            "haven't determined if our phaser energy can generate a stable field. We could alter the "
            "photons with phase discriminators.\n"
            "\n"
            "Communication is not possible.The shuttle has no power.Using the gravitational pull "
            "of a star to slingshot back in time ? We are going to Starbase Montgomery for Engineering "
            "consultations prompted by minor read - out anomalies.Probes have recorded unusual levels "
            "of geological activity in all five planetary systems.Assemble a team.Look at records of the "
            "Drema quadrant.Would these scans detect artificial transmissions as well as natural signals ?\n"
            "\n"
            "Exceeding reaction chamber thermal limit.We have begun power - supply calibration.Force "
            "fields have been established on all turbo lifts and crawlways.Computer, run a level - two "
            "diagnostic on warp - drive systems.Antimatter containment positive.Warp drive within normal "
            "parameters.I read an ion trail characteristic of a freighter escape pod.The bomb had a "
            "molecular - decay detonator.Detecting some unusual fluctuations in subspace frequencies.\n"
            "\n"
            "Unidentified vessel travelling at sub warp speed, bearing 235.7.Fluctuations in energy "
            "readings from it, Captain.All transporters off.A strange set - up, but I'd say the graviton "
            "generator is depolarized. The dark colourings of the scrapes are the leavings of natural "
            "rubber, a type of non-conductive sole used by researchers experimenting with electricity. "
            "The molecules must have been partly de-phased by the anyon beam.\n"
            "\n"
            "Sensors indicate human life forms 30 meters below the planet's surface. Stellar flares "
            "are increasing in magnitude and frequency. Set course for Rhomboid Dronegar 006, warp seven. "
            "There's no evidence of an advanced communication network.Total guidance system failure, with "
            "less than 24 hours' reserve power. Shield effectiveness has been reduced 12 percent. We have "
            "covered the area in a spherical pattern which a ship without warp drive could cross in the "
            "given time.\n"
            "\n"
            "Shields up.I recommend we transfer power to phasers and arm the photon torpedoes.Something "
            "strange on the detector circuit.The weapons must have disrupted our communicators.You saw "
            "something as tasty as meat, but inorganically materialized out of patterns used by our "
            "transporters.Captain, the most elementaryand valuable statement in science, the beginning of "
            "wisdom, is 'I do not know.' All transporters off.\n"
        },
    });

    static constexpr auto HmacData = std::to_array<HmacTest>({
        {
            "804904ec33c31ae4fc8088a42882a247",
            "aa575e0187e496fa3bf742945bea9185dc1497e4",
            "08417f50f9a1898bf747a073e08b26029a70ac883f290d03fc7e19dd8182960b",
            "ea99",
            "6472ab343b204299e25109488e569fcf",
        },
        {
            "5d75abd09ec514a710630cf8d153d63b",
            "732dda769546e7cb63ec6802a5117924bfb04db1",
            "47594a0f535acb2248ecff29271dcc479cc9291fc705c2cf5a2307cb09a9f2f9",
            "58623b23",
            "2b627740915549f96112e7e4bba821c9"
        },
        {
            "80e05eef4dece53c7e29119f66d76201",
            "8a1e32d3be5855a795b8200a44b72a611f05f809",
            "a297b551a9b11521515de87314eeda6d87f29410384944d8e9d5f80fded79a69",
            "3c893f4a570240e1",
            "17b0eacc73e245f726bf9059ea9dfbeb"
        },
        {
            "da64f9bd2b50164ceb9a3bb8a8bd3a7b",
            "ff7a0eda80b87af5ba9131874e74b93e088ce078",
            "378823a5a5ad4193561eb315e1db93ca90211e2145891e1f3950cba29f1997b1",
            "51306f5b0b7b4a93cdafd0216a",
            "72913e81a063d54d908c3c6fbc99f412"
        },
        {
            "17ee2c5616d5940d640d27ac1ccc6722",
            "e00e94901428ae1bc3c72f3cc3dc08fce09ff43b",
            "953d27ea817b445f32fe40375bc9e95164335211d8404a35da03fb4e602148cf",
            "c99e4e0cc1cec01257681b07be7e478e",
            "e14bb6d5e0ad18d132cabf907228de83"
        },
        {
            "954f4e6b5e56f7f144a9b8e425f47b24",
            "c1ff4dff2f684a01bcb46f78f6e6c244ad6dc3a9",
            "8a00f0ebbbcfb86742ef513b5c9bc65844e4b1b582c03c7592c4b3d4e49c6832",
            "The engineering section's critical. Destruction is imminent.",
            "Recommend we adjust shield harmonics to the upper EM band when proceeding. These appear to be some kind of power-wave-guide conduits which allow them to work collectively as they perform ship functions."
        },
        {
            "ecae437dbc93b4b1a39b8a82a53ac356",
            "0f9ead719e24ef23d3e94a5c5d6ef7eebdc42dce",
            "93e3567b4b8c3132fb0cc0151ea586789876e6fac38e3b42b7a4835fadf1d17a",
            "That's why the transporter's biofilters couldn't extract it. The vertex waves show a K-complex corresponding to an REM state. The engineering section's critical. Destruction is imminent.",
            "Exceeding reaction chamber thermal limit. We have begun power-supply calibration. Force fields have been established on all turbo lifts and crawlways. Computer, run a level-two diagnostic on warp-drive systems. Antimatter containment positive. Warp drive within normal parameters. I read an ion trail characteristic of a freighter escape pod. The bomb had a molecular-decay detonator. Detecting some unusual fluctuations in subspace frequencies."
        },
        {
            "b7bf638bbbe688cc17eda9291f7432b2",
            "c6f8c878d81571eb3228265093feb34b5d919ff4",
            "37c4fd4cd4f14ebd2a3ea1823baa461c27724caf59879ea8663a7d2d87f36ed9",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Quisque egestas diam in arcu cursus euismod quis viverra. Pretium vulputate sapien nec sagittis aliquam malesuada bibendum arcu. Malesuada nunc vel risus commodo viverra. Amet risus nullam eget felis eget nunc lobortis mattis aliquam. Bibendum neque egestas congue quisque egestas diam. Nulla malesuada pellentesque elit eget gravida cum sociis natoque. Quam elementum pulvinar etiam non quam lacus suspendisse faucibus interdum. Tempus urna et pharetra pharetra. Sed egestas egestas fringilla phasellus faucibus scelerisque eleifend donec. Pretium lectus quam id leo in vitae turpis massa sed. Ut tellus elementum sagittis vitae et. Ac turpis egestas maecenas pharetra convallis posuere. Sit amet mauris commodo quis imperdiet massa tincidunt nunc. Laoreet id donec ultrices tincidunt arcu non sodales. Porta non pulvinar neque laoreet suspendisse interdum consectetur libero. Blandit turpis cursus in hac habitasse platea dictumst quisque. Non tellus orci ac auctor augue."
        },
    });
};
}
